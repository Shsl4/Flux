#pragma once

#include <Flux/UI/Component.h>

namespace Flux {

    class CursorManager {
        
    public:

        CursorManager() = default;

        virtual ~CursorManager() = default;

        virtual void handleButtonDown(MouseButton button);

        virtual void handleDoubleClick(MouseButton button);

        virtual void handleButtonUp(MouseButton button);

        virtual void handleScroll(Float64 deltaX, Float64 deltaY);

        virtual void handleCursorMoved(Float64 x, Float64 y);

        virtual void handleDrag(Float64 deltaX, Float64 deltaY);

        virtual void handleKeyDown(Key const& key);
        
        virtual void handleKeyUp(Key const& key);

        void focusReactive(Reactive* reactive);
        
        void setComponent(Component* value);

        void notifyDestruction(const Reactive* reactive);
    
    protected:

        void clearHoveredComponent();

        static void setButtonState(Reactive* reactive, MouseButton button, bool value);

        NODISCARD bool buttonFromReactive(const Reactive* value, MouseButton& out) const;

        NODISCARD Point cursorPosition() const;

        Map<MouseButton, Reactive*> stateMap;
        
        Float64 cursorX = 0.0;
        Float64 cursorY = 0.0;
        
        Float64 lastCursorX = 0.0;
        Float64 lastCursorY = 0.0;
        
        Reactive* focused = nullptr;
        Reactive* hovered = nullptr;
        Component* master = nullptr;

    };
    
}
