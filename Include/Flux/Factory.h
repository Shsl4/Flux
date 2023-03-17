#pragma once

#include <Flux/UI/Component.h>
#include <Flux/Application/Window/Window.h>

namespace Flux {

    enum class RenderBackend{

        OpenGL = 0,
        Vulkan,
        Metal,
        DirectX,
        Best

    };

    class Factory final {
    
    public:
        
        Factory();
        
        ~Factory();
        
        template<class ComponentType, typename... Args> requires std::is_base_of_v<Component, ComponentType>
        static ComponentType* createComponent(Args&&... args) {
            auto* component = Allocator<ComponentType>::construct(std::forward<Args>(args)...);
            factory->components.add(component);
            component->initialize();
            return component;
        }
        
        static Window* createWindow(RenderBackend backEnd, String const& name, Int width, Int height);
        
        static void destroyWindow(Window* window);
                
        static bool valid(Component* component);

        static void dispose(Component* component);
        
        NODISCARD FORCEINLINE static SmartArray<Window> const& windows() { return factory->activeWindows; }
        
    private:
        
        static inline Factory* factory = nullptr;
        
        SmartArray<Component> components = {};
        SmartArray<Window> activeWindows = {};

    };

}
