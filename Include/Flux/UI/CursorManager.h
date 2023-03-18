#pragma once

#include <Flux/UI/Component.h>

namespace Flux {

    class CursorManager {
        
    public:

        CursorManager() = default;

        void buttonDown(MouseButton button);

        void doubleClick(MouseButton button) const;

        void buttonUp(MouseButton button);

        void scroll(Float64 xOffset, Float64 yOffset) const;

        void cursorMoved(Float64 x, Float64 y);
        
        void setComponent(Component* value);

        void notifyDestruction(const Reactive* reactive);

    private:

        void clearHoveredComponent();

        NODISCARD bool buttonFromReactive(const Reactive* value, MouseButton& out) const;

        NODISCARD static Component* componentAtPosition(Component* root, Point const& p);

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
