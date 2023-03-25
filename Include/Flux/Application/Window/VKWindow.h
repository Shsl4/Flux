﻿#pragma once

#include <Application/Window/Window.h>

namespace Flux {
    
    class VKWindow : public Window {

    public:

        VKWindow(const String &title, Int windowWidth, Int windowHeight, Component* rootComponent = nullptr);

        ~VKWindow() override;
        
        void draw(Float64 const& deltaTime) override;

        void resized(Int width, Int height) override;

        NODISCARD CursorManager* cursorManager() const override { return this->manager; }

        NODISCARD Component* mainComponent() const override { return this->component; }

    private:

        struct VkContext {
            
            VkInstance instance = nullptr;
            VkDevice device = nullptr;
            VkSurfaceKHR vkSurface = nullptr;
            VkPhysicalDevice physicalDevice = nullptr;
            VkQueue queue = nullptr;
            VkPhysicalDeviceFeatures deviceFeatures = {};
            VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
            
        };

        struct QueueFamilyIndices {
            
            Optional<UInt32> graphicsFamily;
            Optional<UInt32>  presentFamily;

            NODISCARD FORCEINLINE bool valid() const { return graphicsFamily && presentFamily; }

        };

        Component* component = nullptr;
        CursorManager* manager = nullptr;
        SkCanvas* canvas = nullptr;
        
        sk_sp<SkSurface> surface = nullptr;
        GrDirectContext* grContext = nullptr;
        
        Float32 dpiScale = 1.0f;

        Unique<VkContext> vulkanContext = nullptr;

        void initVulkan();

        void setupCanvas(Int width, Int height);

        void createInstance(VkContext* context) const;
        
        void setupPhysicalDevice(VkContext* context) const;

        void createDevice(VkContext* context) const;

        void createSkiaContext(VkContext* context);

        static NODISCARD bool suitableDevice(VkContext* context, VkPhysicalDevice dev);

        static NODISCARD UInt32 rateDevice(VkPhysicalDevice dev);
        
        static NODISCARD QueueFamilyIndices findQueueFamilies(VkContext* context, VkPhysicalDevice device);

    };

}
