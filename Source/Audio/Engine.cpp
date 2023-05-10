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
        this->playerComponent = Factory::createComponent<AudioPlayerComponent>(Point::zero);

        Component* root = Factory::windows().first()->mainComponent();

        const auto topStack = Factory::createComponent<HStack>(Point::zero, Point(root->size().x, 300), VAlignment::center, HAlignment::center);
        const auto bottomStack = Factory::createComponent<HStack>(Point::zero, Point(root->size().x, 300), VAlignment::center, HAlignment::center);

        root->addChild(topStack);
        root->addChild(bottomStack);

        topStack->addChild(playerComponent);
        bottomStack->addChild(wtComponent);
        bottomStack->addChild(filComponent);

        this->wtComponent->linkWaveTable(&waveTable);
        this->playerComponent->linkAudioPlayer(&player);

    }

    void Engine::prepare(Float64 rate, UInt size) {

        player.loadFile(FLUX_RESOURCES"/Audio/100bpm_virtual_riot.wav");
        player.prepare(rate, size);
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

        filComponent->renderer->feedBuffer(audioBuffer[0]);

    }

}
