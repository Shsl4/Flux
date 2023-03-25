#include <Flux/Factory.h>
#include <Flux/Application/Application.h>
#include <Flux/Application/Window/GLWindow.h>
#include <Flux/Application/Window/MetalWindow.h>
#include <Flux/Application/Window/VKWindow.h>

namespace Flux {

    Factory::Factory(){
        nthrowif(factory, "Factory object instantiated multiple times!");
        factory = this;
    }

    Factory::~Factory() {

        while (components.size() > 0) {

            auto *highest = components[0];

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

    Window* Factory::createWindow(Component* component, RenderBackend backEnd, const String &name, Int width, Int height) {

        if(backEnd == RenderBackend::Best){
            
#ifdef __APPLE__
            backEnd = RenderBackend::Metal;
#else
            // TODO Switch to DirectX for windows and Vulkan for Linux when backends are implemented
            backEnd = RenderBackend::OpenGL;
#endif
        }

        Window* window;

        switch (backEnd) {

            case RenderBackend::OpenGL:
                window = Allocator<GLWindow>::construct(name, width, height, component);
                break;
            case RenderBackend::Metal:
            #ifdef __APPLE__
                window = Allocator<MetalWindow>::construct(name, width, height, component);
                break;
            #else
                throw Exceptions::Exception("Unsupported backend.");
            #endif
            case RenderBackend::Vulkan:
                window = Allocator<VKWindow>::construct(name, width, height, component);
                break;
            case RenderBackend::DirectX:
            default:
                throw Exceptions::Exception("Unsupported backend.");
        }

        window->mainComponent()->setColor(ColorScheme::onyx.base);
    
        factory->activeWindows += window;

        return window;

    }

    Window* Factory::createWindow(const RenderBackend backEnd, String const& name, const Int width, const Int height) {
        return createWindow(nullptr, backEnd, name, width, height);
    }

    CursorManager* Factory::createCursorManager() {

        CursorManager* manager = Allocator<CursorManager>::construct();
        factory->managers += manager;
        return manager;
        
    }
    
}
