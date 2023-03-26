#include <Audio/Engine.h>
#include <chrono>

#include <Application/Application.h>

#include <Application/BodePlot.h>

namespace Flux {

    void Engine::opened() {

        
    }

    void Engine::closed() {

    }

    Engine::Engine() {
        
        AudioEngine::listDevices();
        MidiManager::listDevices();

    }

    void Engine::prepare(Float64 rate, UInt size) {
        
        osc.prepare(rate, size);
        fil.prepare(rate, size);
        
        auto* graph = Factory::createComponent<BodePlot>(Point(0, 0), Point(1280, 720));
        graph->setFilter(&fil);

        Factory::windows()[0]->mainComponent()->addChild(graph);
        
    }

    void Engine::receiveMessage(MidiMessage const& message) {

        switch (message.event) {

            case MidiEvent::NoteDown:
                osc.startNote(message);
                break;

            case MidiEvent::NoteUp:
                osc.stopNote(message);
                break;

            case MidiEvent::PitchBend:
                break;

            case MidiEvent::CC:
                break;

            case MidiEvent::Invalid:
                break;
        }

            
    }
    
    void Engine::process(Float64* inputBuffer, Float64* outputBuffer) {

        memset(outputBuffer, 0, sizeof(Float64) * numOutputChannels() * bufferSize());

        const auto audioBuffer = AudioBuffer(outputBuffer, numOutputChannels(), bufferSize());
        
        osc.process(audioBuffer);
        fil.process(audioBuffer);

    }

}
