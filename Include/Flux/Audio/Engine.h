#pragma once

#include <MIDI/MidiManager.h>

#include <Audio/AudioEngine.h>
#include <Audio/AudioPlayer.h>
#include <Audio/WaveTable.h>
#include <Audio/Effects/Filters/IIRFilter.h>

#include <Application/Oscilloscope.h>
#include <Application/WaveTableComponent.h>
#include <Application/FilterComponent.h>
#include <Application/AudioPlayerComponent.h>

namespace Flux {
    
    class Engine : public AudioEngine, public MidiManager {

    public:

        Engine();
        
        void prepare(Float64 rate, UInt size) override;

        void receiveMessage(MidiMessage const& message) override;

        void process(Float64 *inputBuffer, Float64 *outputBuffer) override;
                
    protected:
            
        void opened() override;

        void closed() override;

    private:

        AudioPlayer player;
        WaveTable waveTable;
        Audio::HighPassFilter fil;
        FilterComponent* filComponent;
        WaveTableComponent* wtComponent;
        AudioPlayerComponent* playerComponent;

    };

}
