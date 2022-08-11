#include <Flux/UI/Reactive.h>

#include <Flux/UI/Components/Component.h>

namespace Flux::UserInterface {

    void Reactive::onButtonDown(MouseButton button, Float64 x, Float64 y) {
        
        // Console::logDebug("Received mouse {} down. {}", (UInt)button, getClassName());
            
    }

    void Reactive::onDoubleClick(MouseButton button, Float64 x, Float64 y) {

        // Console::logDebug("Received mouse {} double click. {}", (UInt)button, getClassName());

        if(button==MouseButton::Middle) {
            dynamic_cast<Component*>(this)->discard();
        }
        
    }

    void Reactive::onButtonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget) {
        
        // Console::logDebug("Received mouse {} up. {}", (UInt)button, getClassName());
        
    }
    
    void Reactive::onScroll(Float64 xOffset, Float64 yOffset) {

        // Console::logDebug("Received mouse scroll. {} (x: {}, y: {})", getClassName(), xOffset, yOffset);
            
    }
    
    void Reactive::onDrag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        // Console::logDebug("Received drag. {} (x: {}, y: {})", getClassName(), x, y);
            
    }
    
    void Reactive::onCursorMoved(Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        // Console::logDebug("Received cursor movement. {} (x: {}, y: {})", getClassName(), x, y);
            
    }
    
    void Reactive::onHover() {

        // Console::logDebug("Received hover event. {}", getClassName());
            
    }
    
    void Reactive::endHover() {

        // Console::logDebug("Received end hover event. {}", getClassName());

    }

    void Reactive::onFocus() {

        Console::logRuntime("{} received focus", getClassName());
        
    }
    
    void Reactive::endFocus() {

        Console::logRuntime("{} lost focus", getClassName());
        
    }

}
