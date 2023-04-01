#pragma once

#include <Nucleus/Nucleus.h>

#include <Audio/AudioEngine.h>
#include <MIDI/MidiManager.h>
#include <Audio/Effects/Filters/IIRFilter.h>
#include "Application/Oscilloscope.h"
#include "AudioPlayer.h"
#include "Application/FilterModule.h"

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

        NODISCARD bool playing() const { return velocity > 0.0; }
        
        void process(AudioBuffer<Float64> const& buffer) override {
            
            if(!playing()) return;
            
            for (UInt sample = 0; sample < bufferSize(); ++sample) {
                
                auto proc = processSingle(buffer[0][sample], 0);
                
                for (size_t channel = 0; channel < buffer.channels(); ++channel) {
                    
                    buffer[channel][sample] += proc;
                    
                }

            }
            
        }
        
        Float64 processSingle(Float64 xn, size_t channel) override {
            
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

    class Oscillator : public Audio::AudioObject {
        
    public:

        Oscillator() {
            for(size_t i = 0; i < 16; ++i) {
                voices += Allocator<SawVoice>::construct();
            }
            activeVoices = Map<UInt, SawVoice*>(16);
        }

        void prepare(Float64 rate, UInt size) override {

            for(auto const& voice : voices) {
                voice->prepare(rate, size);
            }
            
        }

        void process(AudioBuffer<Float64> const& buffer) override {

            for(auto const& voice : voices) {
                voice->process(buffer);
            }
            
        }

        void startNote(MidiMessage const& message) {

            if(activeVoices.containsKey(message.noteNumber())) return;
            
            for(auto const& voice : voices) {
                
                if(!voice->playing()) {

                    voice->setFrequency(message.noteFrequency());
                    voice->setVelocity(message.linearValue());
                    activeVoices.add({ message.noteNumber(), voice });
                    break;
                    
                }
                
            }
            
        }

        void stopNote(MidiMessage const& message) {

            while(!activeVoices.containsKey(message.noteNumber())) return;
            
            activeVoices[message.noteNumber()]->setVelocity(0.0);
            activeVoices.removeByKey(message.noteNumber());
            
        }


    private:

        Map<UInt, SawVoice*> activeVoices = {};
        SmartArray<SawVoice> voices = {};
        
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

        Oscillator osc;
        FilterModule module;
        AudioPlayer player;

        Int pressCount = 0;
        UInt lastPressed = 0;
        
    };

}
