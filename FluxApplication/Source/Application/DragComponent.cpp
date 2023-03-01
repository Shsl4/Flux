#include <Application/DragComponent.h>

namespace Flux {

    void DragComponent::onButtonDown(MouseButton button, Float64 x, Float64 y) {
            
        if (button == MouseButton::Left) {

            lastClickPos = Point(f32(x), f32(y)) - position();
                
        }

    }

    void DragComponent::onDrag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {
            
        if (button == MouseButton::Left) { setPosition(Point(f32(x), f32(y)) - lastClickPos); }
            
    }
    
}
