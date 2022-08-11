#include <Application/DragComponent.h>

namespace Flux {

    void DragComponent::onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) {
            
        if (button == UserInterface::MouseButton::Left) {

            lastClickPos = SkVector::Make(f32(x), f32(y)) - getPosition();
                
        }

    }

    void DragComponent::onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {
            
        if (button == UserInterface::MouseButton::Left) { setPosition(SkVector::Make(f32(x), f32(y)) - lastClickPos); }
            
    }
    
}
