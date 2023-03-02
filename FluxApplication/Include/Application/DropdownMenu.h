#pragma once

#include <Flux/UI/Component.h>
#include "Flux/UI/Animation.h"

namespace Flux {

    using namespace UI;

    class ColorAnimation : public Animation {

    public:

        ColorAnimation() = default;
        
        void setTarget(Component* t) {
            this->target = t;
            this->baseColor = t->color();
            this->currentColor = t->color();
        }
        
        void setTargetColor(Color const& color) {
            this->targetColor = color;
        } 
        
    protected:
        
        void restarted() override {
            this->baseColor = currentColor;
        }

        void process(Float64 deltaTime) override {
            this->currentColor = Math::lerp(baseColor, targetColor, f32(progress()));
            target->setColor(currentColor);
        }

    private:
        
        Component* target = nullptr;
        Color baseColor = Colors::white;
        Color currentColor = Colors::white;
        Color targetColor = Colors::white;
        
    };

    class ValueTransition : public Animation {

    public:

        ValueTransition(Float32& ref, Float32& inv) : value(ref), inverse(inv) {
            
        }

        void setRange(Float32 f, Float32 d) {
            this->from = f;
            this->dest = d;
        }
        
    protected:
        
        void process(Float64 deltaTime) override {

            value = Math::easeInOut(from, dest, f32(progress()), 3);
            inverse = Math::easeInOut(dest, from, f32(progress()), 3);
            
        }

    private:

        Float32 from = 0.0f;
        Float32 dest = 0.0f;
        Float32& value;
        Float32& inverse;
        
    };
    
    class DropdownMenu : public Component {
        
    public:

        DropdownMenu();
        
        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void focused() override;

        void lostFocus() override;

        void hovered() override;

        void unHovered() override;

        void buttonDown(MouseButton button, Float64 x, Float64 y) override;

        void buttonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget) override;

    private:

        void updateColor();

        ColorAnimation animation;
        ValueTransition arrowTr;

        Float32 arrowRootY;
        Float32 arrowMaxY;
        
        
        Color defaultColor;
        Color hoverColor;
        Color pressedColor;
        MutableArray<String> choices = {};
        
    };

}
