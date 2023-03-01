#include <Flux/UI/Reactive.h>

namespace Flux::UI {

    void Reactive::onButtonDown(MouseButton button, Float64 x, Float64 y) {
        pressedState[static_cast<size_t>(button)] = true;
    }

    void Reactive::onDoubleClick(MouseButton button, Float64 x, Float64 y) {
    }

    void Reactive::onButtonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget) {
        pressedState[static_cast<size_t>(button)] = false;
    }
    
    void Reactive::onScroll(Float64 xOffset, Float64 yOffset) {
        
    }
    
    void Reactive::onDrag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

    }
    
    void Reactive::onCursorMoved(Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

            
    }
    
    void Reactive::onHover() {
        this->hovered = true;
    }
    
    void Reactive::endHover() {
        this->hovered = false;
    }

    void Reactive::onFocus() {
        this->focused = true;
    }
    
    void Reactive::endFocus() {
        this->focused = false;
    }

}
