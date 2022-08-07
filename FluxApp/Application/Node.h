#pragma once

#include <Flux/UI/Components/Compound.h>
#include "Socket.h"

namespace Flux {

    class Node : public UserInterface::Compound {

    public:
        
        void initialize() override;

        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) override;

        void onDrag(UserInterface::MouseButton button, Float64 x, Float64 y) override;

    private:
        
        SkVector lastClickPos = {};
        
    };
    
}
