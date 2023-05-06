#include <Audio/Engine.h>
#include <Application/Application.h>
#include <Application/NyquistPlot.h>
#include <UI/SceneComponent.h>

namespace Flux {

    void Engine::opened() {

    }

    void Engine::closed() {

    }

    Engine::Engine() : waveTable(Factory::loadWaveFile(FLUX_RESOURCES"/Audio/Basic Shapes.wav")){

        openDevice(0);
        
        this->wtComponent = Factory::createComponent<WaveTableComponent>(Point::zero);
        this->filComponent = Factory::createComponent<FilterComponent>(Point::zero);
        
        Factory::windows()[0]->mainComponent()->addChild(wtComponent);
        Factory::windows()[0]->mainComponent()->addChild(filComponent);

        this->wtComponent->linkWaveTable(&waveTable);

    }

    void Engine::prepare(Float64 rate, UInt size) {

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
        player.play();

        waveTable.prepare(rate, size);
        fil.prepare(rate, size);
        
        filComponent->setFilter(&fil);

    }

    void Engine::receiveMessage(MidiMessage const& message) {

        switch (message.type()) {

            case MidiEvent::noteDown:
                waveTable.startNote(message);
                break;

            case MidiEvent::noteUp:
                waveTable.stopNote(message);
                break;

            case MidiEvent::pitchBend:
                break;

            case MidiEvent::cc:
                break;

            case MidiEvent::invalid:
                break;
        }
            
    }
    
    void Engine::process(Float64* inputBuffer, Float64* outputBuffer) {

        memset(outputBuffer, 0, sizeof(Float64) * numOutputChannels() * bufferSize());

        const auto audioBuffer = AudioBuffer(outputBuffer, numOutputChannels(), bufferSize());

        player.process(audioBuffer);
        
        waveTable.process(audioBuffer);

        fil.process(audioBuffer);

    }

}
