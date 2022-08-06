#pragma once

#include <Flux/Core/Flux.h>

namespace Flux::UserInterface {

    enum class MouseButton {

        Left,
        Right,
        Middle,
        Other
        
    };
    
    class Reactive {
    public:
        
        virtual ~Reactive() = default;

        virtual void onButtonDown(MouseButton button) {}
        
        virtual void onButtonUp(MouseButton button) {}
        
        virtual void onScroll(Float64 xOffset, Float64 yOffset) {}
        
    };
    
}

