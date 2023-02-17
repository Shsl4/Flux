#include <Flux/UI/Reactive.h>

#include <Flux/UI/Components/Component.h>

namespace Flux::UserInterface {

    void Reactive::onButtonDown(MouseButton button, Float64 x, Float64 y) {
        
        // Console::log("Received mouse {} down. {}\n", (UInt)button, Type::name(*this));
            
    }

    void Reactive::onDoubleClick(MouseButton button, Float64 x, Float64 y) {

        // Console::log("Received mouse {} double click. {}\n", (UInt)button, Type::name(*this));

        if(button == MouseButton::Middle) {
            dynamic_cast<Component*>(this)->discard();
        }
        
    }

    void Reactive::onButtonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget) {
        
        // Console::log("Received mouse {} up. {}\n", (UInt)button, Type::name(*this));
        
    }
    
    void Reactive::onScroll(Float64 xOffset, Float64 yOffset) {

        // Console::log("Received mouse scroll. {} (x: {}, y: {})\n", Type::name(*this), xOffset, yOffset);

    }
    
    void Reactive::onDrag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        //Console::log("Received drag. {} (x: {}, y: {})\n", Type::name(*this), x, y);
            
    }
    
    void Reactive::onCursorMoved(Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        //Console::log("Received cursor movement. {} (x: {}, y: {})\n", Type::name(*this), x, y);
            
    }
    
    void Reactive::onHover() {

        //Console::log("Received hover event. {}\n", Type::name(*this));
            
    }
    
    void Reactive::endHover() {

        //Console::log("Received end hover event. {}\n", Type::name(*this));

    }

    void Reactive::onFocus() {
        
    }
    
    void Reactive::endFocus() {
        
    }

}
