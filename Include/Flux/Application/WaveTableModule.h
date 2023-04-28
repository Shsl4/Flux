#pragma once

#include <Application/Module.h>
#include <Audio/WaveTable.h>
#include <UI/Component.h>
#include <Application/RotaryKnob.h>

namespace Flux {

class WaveTableComponent : public Component, public RotaryKnob::Listener {

    public:

        WaveTableComponent();

        void draw(SkCanvas *canvas, Float64 deltaTime) override;

        void initialize() override;

        void setWaveTable(WaveTable* wt);

    void valueChanged(RotaryKnob *knob, Float64 newValue) override;

    private:

        SkPath waveformPath;
        RotaryKnob* positionKnob = nullptr;
        WaveTable* waveTable = nullptr;

    };

    class WaveTableModule : public Module<WaveTable, WaveTableComponent>{

    public:

        void initialize() override;

        void prepare(Float64 rate, size_t size) override;

        void process(const AudioBuffer<Float64> &buffer) override;

        void openWindow() override;

        Window *currentWindow() override;

        WaveTable *processor() override;

        WaveTableComponent *component() override;

    private:

        Window* window = nullptr;
        WaveTableComponent* wtComponent = nullptr;
        Shared<WaveTable> waveTable = nullptr;

    };

}