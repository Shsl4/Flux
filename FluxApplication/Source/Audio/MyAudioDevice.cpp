#include <Audio/MyAudioDevice.h>
#include <chrono>

#include <Application/Application.h>

namespace Flux {

    void MyAudioDevice::outputDeviceOpened() {
            
        filter.initialize(numOutputChannels());
        voice.initialize(numOutputChannels());
        pipeline->setupChannels(numOutputChannels());
        
    }

    void MyAudioDevice::outputDeviceClosed() {

        filter.destroy();
        voice.destroy();
            
    }

    MyAudioDevice::MyAudioDevice() {

        pipeline = Unique<Audio::Pipeline>::make();
        
    }

    void MyAudioDevice::prepare(Float64 rate, UInt size) {
            
        filter.invoke([](auto* obj) {
            obj->setCutoffFrequency(440.0);
        });
        
        filter.prepare(rate, size);
        voice.prepare(rate, size);
        
        pipeline->prepare(rate, size);
/*
        pipeline->addElement<Audio::LPFElement>();
        pipeline->addElement<Audio::HPFElement>();
*/
    }

    void MyAudioDevice::onMidiMessage(MidiMessage const& message) {

        if(message.event == MidiEvent::NoteDown) {

            const Float64 freq = message.noteFrequency();
            const Float64 vel = message.linearValue();

            voice.invoke([freq](auto* obj) {

                obj->setVelocity(1.0);
                obj->setFrequency(freq);
                        
            });

            lastPressed = message.noteNumber();
            ++pressCount;
                
        }

        if(message.event == MidiEvent::NoteUp) {

            --pressCount;

            if(pressCount == 0 || message.noteNumber() == lastPressed) {

                voice.invoke([](auto* obj) {

                    obj->setVelocity(0.0);
                        
                });
                    
            }
                
        }

        if (message.event == MidiEvent::CC) {

            if(message.cc() == 1) {
                    
                const Float64 freq = Math::lerp(20.0, 11000.0, static_cast<Float32>(message.linearValue()));
                    
                filter.invoke([freq](auto* obj) {
                    obj->setCutoffFrequency(freq);
                });
                    
            }

            if(message.cc() == 14) {
                    
                const Float64 res = Math::lerp(0.8, 8.0, static_cast<Float32>(message.linearValue()));

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
            //Console::logDebug("Average: {} ms", total / frame);
            frame = 0.0;
            total = 0.0;
        }
        
    }

}
