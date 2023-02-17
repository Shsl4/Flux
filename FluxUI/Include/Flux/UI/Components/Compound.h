#pragma once

#include <Flux/UI/Components/Component.h>

namespace Flux::UserInterface {
    
    class Compound : public Component {
        
    public:

        template<typename ComponentType>
        ComponentType* addChild() {

            static_assert(std::is_base_of_v<Component, ComponentType>, "ComponentType must be a Component.");

            auto* comp = Allocator<ComponentType>::construct();

            internalAddChild(comp);

            return comp;

        }
        
        template<typename ComponentType>
        ComponentType* addChild(ComponentType* pointer) {

            static_assert(std::is_base_of_v<Component, ComponentType>, "ComponentType must be a Component.");
            internalAddChild(pointer);
            return pointer;
            
        }

        Component* getComponentAtPosition(SkVector const& value);

        NODISCARD Component* getChild(size_t index) const;

        void cleanup() override;

        void draw(SkCanvas* canvas, Float64 deltaTime) override;

    private:

        Component* internalAddChild(Component* component);
        
        SmartArray<Component> children;
        
    };
    
}
