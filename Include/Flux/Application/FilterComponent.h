#pragma once

#include <Application/BodePlot.h>
#include <Application/RotaryKnob.h>

#include "Audio/Effects/Filters/IIRFilter.h"

namespace Flux {
    
    class FilterComponent : public Component, public Slider::Listener, public BodePlot::Listener {
    public:

        FilterComponent(Point const& p);

        void initialize() override;

        void linkFilter(Audio::Filter* newFilter);

        void valueChanged(Slider* knob, Float64 newValue) override;

        void valueChanged(BodePlot* plot, Float64 f, Float64 q) override;

        void setFilter(Audio::LowPassFilter* newFilter);

        BodePlot* renderer = nullptr;

    private:

        RotaryKnob* frequencyKnob = nullptr;
        RotaryKnob* resonanceKnob = nullptr;
        RotaryKnob* mixKnob = nullptr;

        HStack* stack = nullptr;
        
        Audio::Filter* filter = nullptr;

    };
    
}
