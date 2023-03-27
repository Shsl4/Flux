#pragma once

#include <Audio/AudioObject.h>
#include <Application/WaveFile.h>

namespace Flux {

    class AudioPlayer : public Audio::AudioObject {

    public:

        AudioPlayer();

        void prepare(Float64 rate, UInt size) override;

        void process(const AudioBuffer<Float64> &buffer) override;

        void play();

        void pause();

        void stop();

    private:

        bool playing = false;
        Float64 playHead = 0;
        WaveFile wavefile;
        Float64 playRate = 1.0;

    };

}