#pragma once

#include <Flux/UI/Components/Component.h>

namespace Flux {

    class DragComponent : public UserInterface::Component {

    public:

        void onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) override;

        void onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

    private:
        
        SkVector lastClickPos = {};
        
    };
    
}