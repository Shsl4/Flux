#include "DragComponent.h"

namespace Flux {

    void DragComponent::onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) {
            
        if (button == UserInterface::MouseButton::Left) {

            lastClickPos = SkVector(f32(x), f32(y)) - getPosition();
                
        }

    }

    void DragComponent::onDrag(UserInterface::MouseButton button, Float64 x, Float64 y) {
            
        if (button == UserInterface::MouseButton::Left) { setPosition(SkVector(f32(x), f32(y)) - lastClickPos); }
            
    }
    
}
