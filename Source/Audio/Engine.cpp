#include <Audio/Engine.h>
#include <chrono>

#include <Application/Application.h>

namespace Flux {

    void Engine::opened() {

        
    }

    void Engine::closed() {

    }

    Engine::Engine() {

        Flux::AudioEngine::listDevices();
        Flux::MidiManager::listDevices();

    }

    void Engine::prepare(Float64 rate, UInt size) {
    

    }

    void Engine::receiveMessage(MidiMessage const& message) {

        switch (message.event) {

            case MidiEvent::NoteDown:
                break;

            case MidiEvent::NoteUp:
                break;

            case MidiEvent::PitchBend:
                break;

            case MidiEvent::CC:
                break;

            case MidiEvent::Invalid:
                break;
        }

            
    }

    void Engine::process(Float64 *inputBuffer, Float64 *outputBuffer) {


    }

}
