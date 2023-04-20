#include <Flux/UI/Reactive.h>
#include <GLFW/glfw3.h>

namespace Flux {

    Key::Key(Int key, Int mods) : key(key) {

        this->bShift = mods & GLFW_MOD_SHIFT;
        this->bCtrl = mods & GLFW_MOD_CONTROL;
        this->bAlt = mods & GLFW_MOD_ALT;
        this->bCaps = mods & GLFW_MOD_CAPS_LOCK;
        this->bSuper = mods & GLFW_MOD_SUPER;
        this->bNumLock = mods & GLFW_MOD_NUM_LOCK;
        
    }

    void Reactive::buttonDown(MouseButton button, Float64 x, Float64 y) {

    }

    void Reactive::doubleClick(MouseButton button, Float64 x, Float64 y) {

    }

    void Reactive::buttonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget) {

    }
    
    void Reactive::scroll(Float64 deltaX, Float64 deltaY) {
        
    }
    
    void Reactive::drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

    }
    
    void Reactive::cursorMoved(Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

            
    }

    void Reactive::keyDown(Key const& key) {
        // Console::log("{} received {}.\n", Type::name(*this), (key.key));
    }

    void Reactive::keyUp(Key const& key) {
        
    }

    void Reactive::hovered() {
        this->bHovered = true;
    }
    
    void Reactive::unHovered() {
        this->bHovered = false;
    }

    void Reactive::focused() {
        // Console::log("{} earned focus.\n", Type::name(*this));
        this->bFocused = true;
    }
    
    void Reactive::lostFocus() {
        // Console::log("{} lost focus.\n", Type::name(*this));
        this->bFocused = false;
    }

    void Reactive::setReactive(bool state) {
        this->bActive = state;
    }
    
}
