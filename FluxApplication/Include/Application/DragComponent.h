#pragma once

#include <Flux/UI/Component.h>

namespace Flux {

    using namespace UI;
    
    class DragComponent : public Component {

    public:

        void onButtonDown(MouseButton button, Float64 x, Float64 y) override;

        void onDrag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

    private:
        
        Point lastClickPos = {};
        
    };
    
}