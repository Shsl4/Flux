#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <Application/Window/VKWindow.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/gpu/GrDirectContext.h>
#include <skia/include/gpu/vk/GrVkBackendContext.h>
#include <skia/include/gpu/vk/GrVkTypes.h>
#include <skia/include/gpu/vk/VulkanExtensions.h>

#include <Factory.h>
#include <set>

#ifdef SK_VULKAN

namespace Flux {

    VKWindow::VKWindow(const String& title, Int windowWidth, Int windowHeight, Component* rootComponent) {

        if(!glfwVulkanSupported())
            throw Exceptions::Exception("Trying to create a Vulkan window, but vulkan is not supported!");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        
        this->handle = glfwCreateWindow(windowWidth, windowHeight, title.begin().get(), nullptr, nullptr);

        this->vulkanContext = Unique<VkContext>::make();
        
        initVulkan();

        this->component = Factory::createComponent<Component>(Point(0, 0), Point(f32(windowWidth), f32(windowHeight)));
        this->component->addChild(rootComponent);
        
        if(auto* cast = dynamic_cast<CursorManager*>(rootComponent)) {
            
            this->manager = cast;
            this->manager->setComponent(rootComponent);
            
        }
        else {
            
            this->manager = Factory::createCursorManager();
            this->manager->setComponent(this->component);
            
        }

        setupCanvas(windowWidth, windowHeight);
        
    }

    VKWindow::~VKWindow() {

        const auto destroyDevice = reinterpret_cast<PFN_vkDestroyDevice>(glfwGetInstanceProcAddress(nullptr, "vkDestroyDevice"));
        const auto destroySurface = reinterpret_cast<PFN_vkDestroySurfaceKHR>(glfwGetInstanceProcAddress(nullptr, "vkDestroySurfaceKHR"));
        const auto destroyInstance = reinterpret_cast<PFN_vkDestroyInstance>(glfwGetInstanceProcAddress(nullptr, "destroyInstance"));

        destroySurface(vulkanContext->instance, vulkanContext->vkSurface, nullptr);
        destroyDevice(vulkanContext->device, nullptr);
        destroyInstance(vulkanContext->instance, nullptr);

    }
    
    void VKWindow::draw(Float64 const& deltaTime) {


        
    }

    void VKWindow::initVulkan() {

        createInstance(vulkanContext.pointer());
        setupPhysicalDevice(vulkanContext.pointer());
        createDevice(vulkanContext.pointer());
        createSkiaContext(vulkanContext.pointer());
        
    }
    
    void VKWindow::createSkiaContext(VkContext* context) {

        const auto enumExtensions = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(glfwGetInstanceProcAddress(nullptr, "vkEnumerateDeviceExtensionProperties"));

        skgpu::VulkanExtensions skExts;

        const std::function fn = [](const char* name, VkInstance inst, VkDevice) {

            return glfwGetInstanceProcAddress(inst, name);
            
        };
        
        UInt32 instCount;
        const char** extensions = glfwGetRequiredInstanceExtensions(&instCount);
        
        UInt32 count = 0;
        std::vector<char*> deviceExtensions;
        enumExtensions(context->physicalDevice, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> props(count);
        enumExtensions(context->physicalDevice, nullptr, &count, props.data());
        
        for(auto& [extensionName, specVersion] : props) {
            deviceExtensions.push_back(extensionName);
        }
        
        skExts.init(fn, context->instance, context->physicalDevice, instCount,
            extensions, count, deviceExtensions.data());
        
        auto itf = GrVkBackendContext();
        
        itf.fDevice = context->device;
        itf.fInstance = context->instance;
        itf.fPhysicalDevice = context->physicalDevice;
        itf.fQueue = context->queue;
        itf.fMaxAPIVersion = VK_API_VERSION_1_3;
        itf.fGraphicsQueueIndex = 0;
        itf.fVkExtensions = &skExts;
        itf.fGetProc = fn;
        itf.fDeviceFeatures = &context->deviceFeatures;
        itf.fDeviceFeatures2 = &context->deviceFeatures2;
        
        this->grContext = GrDirectContext::MakeVulkan(itf).release();

        if(!this->grContext) throw Exceptions::Exception("Failed to create Skia Vulkan context.");
        
    }

    bool VKWindow::suitableDevice(VkContext* context, const VkPhysicalDevice dev) {

        const auto devProps = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(glfwGetInstanceProcAddress(nullptr, "vkGetPhysicalDeviceProperties"));
        const auto devFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(glfwGetInstanceProcAddress(nullptr, "vkGetPhysicalDeviceFeatures"));
      
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        
        devProps(dev, &deviceProperties);
        devFeatures(dev, &deviceFeatures);
        
        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader && findQueueFamilies(context, dev).valid();
        
    }

    UInt32 VKWindow::rateDevice(VkPhysicalDevice dev) {

        const auto devProps = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(glfwGetInstanceProcAddress(nullptr, "vkGetPhysicalDeviceProperties"));
        const auto devFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(glfwGetInstanceProcAddress(nullptr, "vkGetPhysicalDeviceFeatures"));
        
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        
        devProps(dev, &deviceProperties);
        devFeatures(dev, &deviceFeatures);
        
        UInt32 score = 0;

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        // Application can't function without geometry shaders
        if (!deviceFeatures.geometryShader) {
            return 0;
        }

        return score;
        
    }

    void VKWindow::resized(Int width, Int height) {}
    
    void VKWindow::setupCanvas(Int width, Int height) {
        
        
    }

    void VKWindow::createInstance(VkContext* context) const {

        const auto createVkInstance = reinterpret_cast<PFN_vkCreateInstance>(glfwGetInstanceProcAddress(nullptr, "vkCreateInstance"));
        
        UInt32 instCount;
        const char** extensions = glfwGetRequiredInstanceExtensions(&instCount);

        VkApplicationInfo appInfo {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Flux";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Flux";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;
        
        VkInstanceCreateInfo instanceInfo {};
        
        instanceInfo.enabledExtensionCount = instCount;
        instanceInfo.ppEnabledExtensionNames = extensions;
        instanceInfo.pApplicationInfo = &appInfo;
        
        if(createVkInstance(&instanceInfo, nullptr, &context->instance) != VK_SUCCESS) {
            throw Exceptions::Exception("Failed to create vulkan instance!");
        }
        
        if(glfwCreateWindowSurface(context->instance, this->handle, nullptr, &context->vkSurface) != VK_SUCCESS) {
            throw Exceptions::Exception("Failed to create vulkan surface!");
        }
        
    }

    void VKWindow::setupPhysicalDevice(VkContext* context) const {

        const auto enumerateDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(glfwGetInstanceProcAddress(nullptr, "vkEnumeratePhysicalDevices"));

        UInt32 count = 0;
        enumerateDevices(context->instance, &count, nullptr);
        std::vector<VkPhysicalDevice> devices(count);
        enumerateDevices(context->instance, &count, devices.data());
        
        Map<UInt32, VkPhysicalDevice> scoreMap;
        size_t selectedIndex = 0;

        for (size_t i = 0; i < count; ++i) {
            
            if(VkPhysicalDevice currentDevice = devices[i]; suitableDevice(context, currentDevice)) {

                const UInt32 score = rateDevice(currentDevice);
                
                scoreMap.add(score, currentDevice);

                if(score > scoreMap.get(selectedIndex).key()) selectedIndex = i;
                
            }
            
        }

        if (scoreMap.size() == 0) {
            throw Exceptions::Exception("Failed to find any suitable rendering device!");
        }

        context->physicalDevice = scoreMap.get(selectedIndex).value();
        
    }

    VKWindow::QueueFamilyIndices VKWindow::findQueueFamilies(VkContext* context, VkPhysicalDevice device) {

        const auto familyProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(glfwGetInstanceProcAddress(nullptr, "vkGetPhysicalDeviceQueueFamilyProperties"));
        const auto supportsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(glfwGetInstanceProcAddress(nullptr, "vkGetPhysicalDeviceSurfaceSupportKHR"));

        QueueFamilyIndices indices = {};

        UInt32 queueFamilyCount = 0;
        familyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        familyProperties(device, &queueFamilyCount, queueFamilies.data());
        
        for (UInt32 i = 0; i < queueFamilyCount; ++i) {
            
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                
                indices.graphicsFamily = Optional(i);

                UInt32 result = 0;
                supportsKHR(device, i, context->vkSurface, &result);

                if(result) {
                    indices.presentFamily = Optional(result);
                }
                
            }
            
        }
        
        return indices;
        
    }
    
    void VKWindow::createDevice(VkContext* context) const {

        const auto createDevice = reinterpret_cast<PFN_vkCreateDevice>(glfwGetInstanceProcAddress(nullptr, "vkCreateDevice"));
        const auto enumExtensions = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(glfwGetInstanceProcAddress(nullptr, "vkEnumerateDeviceExtensionProperties"));
        const auto devQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(glfwGetInstanceProcAddress(nullptr, "vkGetDeviceQueue"));
        const auto devFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(glfwGetInstanceProcAddress(nullptr, "vkGetPhysicalDeviceFeatures"));
        const auto devFeatures2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(glfwGetInstanceProcAddress(nullptr, "vkGetPhysicalDeviceFeatures2"));
        
        constexpr float queuePriority = 1.0f;

        QueueFamilyIndices indices = findQueueFamilies(context, context->physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set uniqueQueueFamilies = { *indices.graphicsFamily, *indices.presentFamily };

        for (uint32_t queueFamily : uniqueQueueFamilies) {
            
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
            
        }

        std::vector<char*> deviceExtensions;

        UInt32 count = 0;
        enumExtensions(context->physicalDevice, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> props(count);
        enumExtensions(context->physicalDevice, nullptr, &count, props.data());
        
        for(auto& [extensionName, specVersion] : props) {
            deviceExtensions.push_back(extensionName);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        
        VkDeviceCreateInfo deviceInfo {};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount    = static_cast<UInt32>(queueCreateInfos.size());
        deviceInfo.pQueueCreateInfos       = queueCreateInfos.data();
        deviceInfo.enabledExtensionCount   = count;
        deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceInfo.pEnabledFeatures = &deviceFeatures;
        
        if(createDevice(context->physicalDevice, &deviceInfo, nullptr, &context->device) != VK_SUCCESS) {
            throw Exceptions::Exception("Failed to create vulkan device!");
        }

        devQueue(context->device, *indices.presentFamily, 0, &context->queue);
        devFeatures(context->physicalDevice, &context->deviceFeatures);
        devFeatures2(context->physicalDevice, &context->deviceFeatures2);
        
    }
    
}

#endif
