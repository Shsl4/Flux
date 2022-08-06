#include "MyAudioDevice.h"

namespace Flux {

    void MyAudioDevice::onOutputDeviceOpened() {
            
        filter.initialize(getOutputChannelCount());
        voice.initialize(getOutputChannelCount());
            
    }

    void MyAudioDevice::onOutputDeviceClosed() {

        filter.destroy();
        voice.destroy();
            
    }

    void MyAudioDevice::prepare(Float64 rate, UInt size) {
            
        filter.invoke([](auto* obj) {
            obj->setCutoffFrequency(440.0);
        });

        filter.prepare(rate, size);
        voice.prepare(rate, size);
            
    }

    void MyAudioDevice::onMidiMessage(MidiMessage const& message) {

        if(message.event == MidiEvent::NoteDown) {

            const Float64 freq = message.getNoteFrequency();
            const Float64 vel = message.getLinearValue();

            voice.invoke([freq](auto* obj) {

                obj->setVelocity(1.0);
                obj->setFrequency(freq);
                        
            });

            lastPressed = message.getNoteNumber();
            ++pressCount;
                
        }

        if(message.event == MidiEvent::NoteUp) {

            --pressCount;

            if(pressCount == 0 || message.getNoteNumber() == lastPressed) {

                voice.invoke([](auto* obj) {

                    obj->setVelocity(0.0);
                        
                });
                    
            }
                
        }

        if (message.event == MidiEvent::CC) {

            if(message.getCC() == 1) {
                    
                const Float64 freq = Math::lerp(20.0, 11000.0, static_cast<Float32>(message.getLinearValue()));
                    
                filter.invoke([freq](auto* obj) {
                    obj->setCutoffFrequency(freq);
                });
                    
            }

            if(message.getCC() == 14) {
                    
                const Float64 res = Math::lerp(0.8, 8.0, static_cast<Float32>(message.getLinearValue()));

                filter.invoke([res](auto* obj) {
                    obj->setResonance(res);
                });
                    
            }
                
        }
            
    }

}
