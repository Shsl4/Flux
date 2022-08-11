#pragma once

#include <map>
#include <Flux/Core/Flux.h>
#include "Reactive.h"
#include "Components/Compound.h"

namespace Flux::UserInterface {

    class CursorManager {
        
    public:

        CursorManager();

        void onButtonDown(MouseButton button);

        void onDoubleClick(MouseButton button) const;

        void onButtonUp(MouseButton button);

        void onScroll(Float64 xOffset, Float64 yOffset) const;

        void onCursorMoved(Float64 x, Float64 y);
        
        void setCompound(Compound* value);

        void notifyDestruction(const Reactive* reactive);

        static CursorManager* getCursorManager();

    private:

        void clearHoveredComponent();

        bool getButtonFromReactive(const Reactive* value, MouseButton& out);

        NODISCARD SkVector getCursorPosition() const;

        std::map<MouseButton, Reactive*> stateMap;
        
        Float64 cursorX = 0.0;
        Float64 cursorY = 0.0;
        
        Float64 lastCursorX = 0.0;
        Float64 lastCursorY = 0.0;
        
        Reactive* focused = nullptr;
        Reactive* hovered = nullptr;
        Compound* master = nullptr;

        static inline CursorManager* manager = nullptr;
        
    };
    
}
