#include <Reactive.h>

namespace Flux::UserInterface {

    void Reactive::onButtonDown(MouseButton button, Float64 x, Float64 y) {
            
        // Console::logDebug("Received mouse {} down. {}", (UInt)button, getClassName());
            
    }
    
    void Reactive::onButtonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget) {
        
        // Console::logDebug("Received mouse {} up. {}", (UInt)button, getClassName());
        
    }
    
    void Reactive::onScroll(Float64 xOffset, Float64 yOffset) {

        // Console::logDebug("Received mouse scroll. {} (x: {}, y: {})", getClassName(), xOffset, yOffset);
            
    }
    
    void Reactive::onDrag(MouseButton button, Float64 x, Float64 y) {

        // Console::logDebug("Received drag. {} (x: {}, y: {})", getClassName(), x, y);
            
    }
    
    void Reactive::onCursorMoved(Float64 x, Float64 y) {

        // Console::logDebug("Received cursor movement. {} (x: {}, y: {})", getClassName(), x, y);
            
    }
    
    void Reactive::onHover() {

        // Console::logDebug("Received hover event. {}", getClassName());
            
    }
    
    void Reactive::endHover() {

        // Console::logDebug("Received end hover event. {}", getClassName());

    }
    
}