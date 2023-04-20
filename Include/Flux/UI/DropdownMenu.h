#pragma once

#include <Flux/UI/Component.h>
#include "Flux/UI/Animation.h"
#include <Flux/UI/Stack.h>
#include <Flux/UI/Button.h>

namespace Flux {

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

        DropdownMenu(Point const& p, Point const& s);
        
        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void focused() override;

        void lostFocus() override;

        void setChoices(MutableArray<String> const& array);

        void choiceCallback(Function<void(size_t)> const& f);

        void initialize() override;

    protected:

        void choiceSelected(size_t i);

        void modified() override;

    private:

        ValueTransition arrowTr;

        Float32 arrowRootY;
        Float32 arrowMaxY;
                
        ColorScheme scheme = ColorScheme::emerald;

        size_t currentSelection = 0;
        MutableArray<String> choices = {};

        Button* button = nullptr;
        VStack* stack = nullptr;

        Function<void(size_t)> callback;


    };

}
