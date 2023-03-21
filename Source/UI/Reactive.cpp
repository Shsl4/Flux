#include <Flux/UI/Reactive.h>

namespace Flux {

    void Reactive::buttonDown(MouseButton button, Float64 x, Float64 y) {

    }

    void Reactive::doubleClick(MouseButton button, Float64 x, Float64 y) {

    }

    void Reactive::buttonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget) {

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
        Console::log("{} earned focus.\n", Type::name(*this));
        this->bFocused = true;
    }
    
    void Reactive::lostFocus() {
        Console::log("{} lost focus.\n", Type::name(*this));
        this->bFocused = false;
    }

    void Reactive::setReactive(bool state) {
        this->bActive = state;
    }
}
