#pragma once

#include "AudioEngine.h"
#include "../MIDI/MidiManager.h"

#include <Nucleus/Nucleus.h>
#include <Flux/Audio/Effects/MultiProcessor.h>
#include <Flux/Audio/Effects/Filters/IIRFilter.h>
#include <Audio/Pipeline/Pipeline.h>

using namespace Nucleus;

namespace Flux {

    class SawVoice : public Audio::AudioEffect {

    public:

        void prepare(Float64 rate, UInt size) override {

            AudioObject::prepare(rate, size);
            phaseIncrement = (Math::pi<Float64> * 2.0 / sampleRate()) * frequency;
            
        }
        
        void setFrequency(Float64 value) {

            this->frequency = value;
            phaseIncrement = (Math::pi<Float64> * 2.0 / sampleRate()) * frequency;
            
        }

        void setVelocity(Float64 value) {

            this->velocity = Math::clamp(value, 0.0, 1.0);
            
        }

        bool process(Float64* buffer) override {

            for (UInt sample = 0; sample < bufferSize(); ++sample) {
                buffer[sample] = processSingle(buffer[sample]);
            }

            return true;
            
        }
        
        Float64 processSingle(Float64 xn) override {
            
            auto r  = signal() * velocity;
            updatePhase();
            return r;
            
        }

        void updatePhase() {

            auto twoPi = Math::pi<Float64> * 2.0;
            phase += phaseIncrement;

            if (phase >= twoPi) {
                phase -= twoPi;
            }
            if (phase < 0.0) {
                phase += twoPi;
            }

        }

        Float64 signal() {

            return (2.0 * (phase * (1.0 / (2.0 * Math::pi<Float64>)))) - 1.0;
            
        }
        
        Float64 frequency = 440.0;
        Float64 velocity = 0.0;
        Float64 phase = 0.0;
        Float64 phaseIncrement = 0.0;
        
    };
    
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

        Int pressCount = 0;
        UInt lastPressed = 0;
        
    };

}
