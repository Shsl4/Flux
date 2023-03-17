#include <Flux/Factory.h>
#include <Flux/Application/Application.h>
#include <Flux/Application/Window/OpenGLWindow.h>
#include <Flux/Application/Window/MetalWindow.h>

namespace Flux {

    Factory::Factory(){
        nthrowif(factory, "Factory object instantiated multiple times!");
        factory = this;
    }

    Factory::~Factory() {

        while (components.size() > 0) {
        
            auto* highest = components[0];

            while (highest->parent()) {
                highest = highest->parent();
            }
        
            highest->dispose();
        
        }
    
    }

    bool Factory::valid(Component* component) { return factory->components.contains(component); }

    void Factory::dispose(Component* component) {

        if(!valid(component)) return;
    
        nthrowif(component->children().size() > 0, "Trying to destroy a component that has active children. "
                 "Make sure to use Component::dispose() to destroy components.");

        Console::log("Disposing {}.\n", Type::name(*component));
    
        factory->components.remove(component);
    
    }

    void Factory::destroyWindow(Window* window){
    
        if (factory->activeWindows[0] == window) {
            Application::instance()->stop();
        }
        
        factory->activeWindows.remove(window);
        
    }

    Window* Factory::createWindow(RenderBackend backEnd, const String &name, Int width, Int height) {

        if(backEnd == RenderBackend::Best){
            
#ifdef __APPLE__
            backEnd = RenderBackend::Metal;
#elif defined(WIN32)
            backEnd = RenderBackend::DirectX;
#else
            backEnd = glfwVulkanSupported() ? RenderBackend::Vulkan : RenderBackend::OpenGL;
#endif
        }

        Window* window;

        switch (backEnd) {

            case RenderBackend::OpenGL:
                window = Allocator<OpenGLWindow>::construct(name, width, height);
                break;
            case RenderBackend::Metal:
            #ifdef __APPLE__
                window = Allocator<MetalWindow>::construct(name, width, height);
            #else
                throw Exceptions::Exception("Unsupported backend.");
            #endif
                break;
            case RenderBackend::Vulkan:
            case RenderBackend::DirectX:
            default:
                throw Exceptions::Exception("Unsupported backend.");
        }

        window->mainView()->setColor(Color::fromHex(0x151515ff));
    
        factory->activeWindows += window;

        return window;

    }

}
