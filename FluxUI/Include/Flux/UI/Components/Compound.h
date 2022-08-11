#pragma once

#include <Flux/UI/Components/Component.h>

namespace Flux::UserInterface {
    
    class Compound : public Component {
        
    public:

        template<typename ComponentType>
        WeakPointer<ComponentType> addChild() {

            static_assert(std::is_base_of_v<Component, ComponentType>, "ComponentType must be a Component.");
            
            auto pointer = SharedPointer<ComponentType>::make();

            internalAddChild(pointer);

            return pointer.weak();
            
        }
        
        template<typename ComponentType>
        void addChild(SharedPointer<ComponentType> pointer) {

            static_assert(std::is_base_of_v<Component, ComponentType>, "ComponentType must be a Component.");
            internalAddChild(pointer);
            
        }

        Component* getComponentAtPosition(SkVector const& value);

        NODISCARD Component* getChild(size_t index) const;

        void cleanup() override;

        void draw(SkCanvas* canvas, Float64 deltaTime) override;

    private:

        void internalAddChild(SharedPointer<Component> const& component);
        
        OwnedArray<Component> children;
        
    };
    
}
