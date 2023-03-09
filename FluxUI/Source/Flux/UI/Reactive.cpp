#include <Flux/UI/Reactive.h>

namespace Flux::UI {

    void Reactive::buttonDown(MouseButton button, Float64 x, Float64 y) {
        pressedState[static_cast<size_t>(button)] = true;
    }

    void Reactive::doubleClick(MouseButton button, Float64 x, Float64 y) {
    }

    void Reactive::buttonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget) {
        pressedState[static_cast<size_t>(button)] = false;
    }
    
    void Reactive::scroll(Float64 xOffset, Float64 yOffset) {
        
    }
    
    void Reactive::drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

    }
    
    void Reactive::cursorMoved(Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

            
    }
    
    void Reactive::hovered() {
        this->bHovered = true;
    }
    
    void Reactive::unHovered() {
        this->bHovered = false;
    }

    void Reactive::focused() {
        this->bFocused = true;
    }
    
    void Reactive::lostFocus() {
        this->bFocused = false;
    }

    void Reactive::setActive(bool state) {
        this->bActive = state;
    }
}
