#pragma once

#include <Application/Slider.h>

namespace Flux {

    class RotaryKnob : public Slider {

    public:

        RotaryKnob(Point const& p, Float32 radius);
        
        void drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

        void draw(Graphics const& graphics) override;
    
    protected:
        
        void refreshProgress() override;

    private:

        Float64 rotation = 0.0f;
        
    };

}