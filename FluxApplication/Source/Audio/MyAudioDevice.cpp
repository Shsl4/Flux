#include <Audio/MyAudioDevice.h>
#include <chrono>

#include <Application/Application.h>
#include "Flux/Core/Tools/Console/Console.h"

namespace Flux {

    void MyAudioDevice::onOutputDeviceOpened() {
            
        filter.initialize(getOutputChannelCount());
        voice.initialize(getOutputChannelCount());
        pipeline->setupChannels(getOutputChannelCount());
        
    }

    void MyAudioDevice::onOutputDeviceClosed() {

        filter.destroy();
        voice.destroy();
            
    }

    MyAudioDevice::MyAudioDevice() {

        pipeline = UniquePointer<Audio::Pipeline>::make();
        pipeline->setPipelineView(Application::getMasterView());
        
    }
    Audio::LowPassFilter fil;

    void MyAudioDevice::prepare(Float64 rate, UInt size) {
            
        filter.invoke([](auto* obj) {
            obj->setCutoffFrequency(440.0);
        });
        
        filter.prepare(rate, size);
        voice.prepare(rate, size);
        
        pipeline->prepare(rate, size);

        pipeline->addElement<Audio::LPFElement>();
        
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

    Float64 frame = 0.0;
    Float64 total = 0.0;
    
    void MyAudioDevice::process(Float64* buffer) {

        voice.process(buffer);
        
        using std::chrono::high_resolution_clock;
        using std::chrono::duration_cast;
        using std::chrono::duration;
        using std::chrono::milliseconds;

        auto t1 = high_resolution_clock::now();

        pipeline->render(buffer);

        auto t2 = high_resolution_clock::now();
        
        duration<double, std::milli> ms_double = t2 - t1;

        total += ms_double.count();
        frame += 1.0;

        if(frame >= 250) {
            Console::logDebug("Average: {} ms", total / frame);
            frame = 0.0;
            total = 0.0;
        }
        
    }

}
