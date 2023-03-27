#include <Audio/AudioPlayer.h>

namespace Flux {

    void AudioPlayer::process(const AudioBuffer<Float64> &buffer) {

        if(playing){

            const size_t max = Math::min(buffer.bufferSize(), wavefile.samplesPerChannel - static_cast<size_t>(playHead));

            for(size_t sample = 0; sample < max; ++sample){

                for (size_t channel = 0; channel < buffer.channels(); ++channel) {
                    buffer[channel][sample] = wavefile.buffers[channel][static_cast<size_t>(playHead)];
                }

                playHead += playRate * wavefile.sampleRate / sampleRate();

            }

            if(static_cast<size_t>(playHead) >= wavefile.samplesPerChannel) stop();

        }

    }

    void AudioPlayer::play() {
        this->playing = true;
    }

    void AudioPlayer::pause() {
        this->playing = false;
    }

    void AudioPlayer::stop() {
        this->playing = false;
        this->playHead = 0;
    }

    AudioPlayer::AudioPlayer() : wavefile(String::format("{}/Audio/100bpm_virtual_riot.wav", FLUX_RESOURCES)) {

    }

    void AudioPlayer::prepare(Float64 rate, UInt size) {
        AudioObject::prepare(rate, size);
    }

}
