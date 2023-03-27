#include <Audio/Engine.h>
#include <chrono>

#include <Application/Application.h>

#include <Application/BodePlot.h>

#include "Application/NyquistPlot.h"
#include "UI/SceneComponent.h"
#include "Application/Oscilloscope.h"

namespace Flux {

    void Engine::opened() {

        
    }

    void Engine::closed() {

    }

    Engine::Engine() {

    }

    void Engine::prepare(Float64 rate, UInt size) {
        
        osc.prepare(rate, size);
        fil.prepare(rate, size);

        auto* scene = Factory::windows()[0]->mainComponent()->firstComponentOf<SceneComponent>();
        
        auto* graph = Factory::createComponent<BodePlot>(Point(0, 0), Point(1280 / 2, 720 / 2));
        graph->setFilter(&fil);
        
        auto* nyq = Factory::createComponent<NyquistPlot>(Point(1280 / 2, 0), Point(720 / 2, 720 / 2));
        nyq->setFilter(&fil);

        this->oscPlot = Factory::createComponent<Oscilloscope>(Point(0, 720 / 2), Point(1280 / 2, 720 / 2));

        graph->setCallback([nyq](BodePlot* plot) {
           nyq->recalculateResponse(); 
        });
        
        scene->addChild(graph);
        scene->addChild(nyq);
        scene->addChild(oscPlot);

        player.prepare(rate, size);

        player.play();

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

        player.process(audioBuffer);
        osc.process(audioBuffer);
        fil.process(audioBuffer);

        // TODO: Fix heap use after free when closing window
        oscPlot->feed(audioBuffer);

    }

}
