#pragma once

#include <Factory.h>
#include <UI/Stack.h>
#include <Application/RotaryKnob.h>
#include <Application/Oscilloscope.h>
#include <Audio/WaveTable.h>

namespace Flux {

    class WaveTableComponent : public Component, public Slider::Listener {

        class WaveTableRenderer : public Component {

        public:

            WaveTableRenderer(Point const& p, Point const& s);

            void draw(const Graphics &graphics) override;

            void calculateWaveform(MutableArray<Float64> const& data);

            void initialize() override;

        protected:
            void modified() override;

        private:

            ColorScheme scheme = ColorScheme::darkScheme(Color::tintPurple);
            Path path;

        };

    public:

        WaveTableComponent(Point const& pos);

        void initialize() override;

        void valueChanged(Slider *knob, Float64 newValue) override;

        void linkWaveTable(WaveTable* wt);

    private:

        RotaryKnob* frameKnob = nullptr;
        RotaryKnob* phaseKnob = nullptr;
        RotaryKnob* panKnob = nullptr;
        RotaryKnob* gainKnob = nullptr;

        RotaryKnob* attackKnob = nullptr;
        RotaryKnob* decayKnob = nullptr;
        RotaryKnob* sustainKnob = nullptr;
        RotaryKnob* releaseKnob = nullptr;
        
        WaveTableRenderer* renderer = nullptr;

        VStack* stack = nullptr;

        WaveTable* waveTable = nullptr;

    };

}