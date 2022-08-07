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

        Component* getComponentAtPosition(SkVector const& value);

        Component* getChild(size_t index) const;
        
        void draw(SkCanvas* canvas, Float64 deltaTime) override;

    private:

        void internalAddChild(SharedPointer<Component> const& drawable);
        
        OwnedArray<Component> children;
        
    };
    
}
