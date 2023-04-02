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

    Engine::Engine() : table(Factory::loadWaveFile(FLUX_RESOURCES"/Audio/Basic Shapes.wav")){
        module.initialize();
        // this->scope = Factory::createComponent<Oscilloscope>(Point::zero, Point(500,300));
    }

    void Engine::prepare(Float64 rate, UInt size) {
        
        osc.prepare(rate, size);
        module.prepare(rate, size);
        table.prepare(rate, size);

        module.openWindow();
        // Factory::windows()[0]->mainComponent()->addChild(scope);

        player.loadFile(FLUX_RESOURCES"/Audio/100bpm_virtual_riot.wav");
        player.prepare(rate, size);

        player.resample().write(FLUX_RESOURCES"/Audio/copy.wav");
        player.transpose(2);
        player.resample().write(FLUX_RESOURCES"/Audio/transposed.wav");

        player.transpose(0);
        player.setReverse(true);
        player.resample().write(FLUX_RESOURCES"/Audio/reversed.wav");
        
        player.setReverse(false);
        player.setStartTime(21.25);
        player.setEndTime(21.50);
        player.resample().write(FLUX_RESOURCES"/Audio/resampled.wav");

        player.resetStartAndEnd();
        player.setLooping(true);
        // player.play();

    }

    void Engine::receiveMessage(MidiMessage const& message) {

        switch (message.event) {

            case MidiEvent::NoteDown:
                table.startNote(message);
                break;

            case MidiEvent::NoteUp:
                table.stopNote(message);
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
        table.process(audioBuffer);
        module.process(audioBuffer);
        // scope->feed(audioBuffer);

    }

}
