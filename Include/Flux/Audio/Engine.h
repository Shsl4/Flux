#pragma once

#include <Nucleus/Nucleus.h>

#include <Audio/AudioEngine.h>
#include <MIDI/MidiManager.h>
#include <Application/Oscilloscope.h>
#include <Audio/AudioPlayer.h>
#include <Audio/WaveTable.h>

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

    };

}
