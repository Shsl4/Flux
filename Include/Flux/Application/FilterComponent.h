#pragma once

#include <Application/BodePlot.h>
#include <Application/RotaryKnob.h>

namespace Flux {

    class FilterComponent : public Component, public RotaryKnob::Listener, public BodePlot::Listener{

    public:

        FilterComponent();

        void initialize() override;

        void valueChanged(BodePlot *plot, Float64 f, Float64 q) override;

        void valueChanged(RotaryKnob *knob, Float64 newValue) override;

        NODISCARD FORCEINLINE BodePlot* plot() const { return this->filterPlot; }

    private:

        BodePlot* filterPlot = nullptr;
        RotaryKnob* frequencyKnob = nullptr;
        RotaryKnob* resonanceKnob = nullptr;


    };

}