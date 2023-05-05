#pragma once

#include <Audio/AudioProcessor.h>
#include <Application/WaveFile.h>

namespace Flux {

    class AudioPlayer : public AudioProcessor {

    public:

        AudioPlayer();

        void prepare(Float64 rate, UInt size) override;

        void process(const AudioBuffer<Float64> &buffer) override;

        void play();

        void pause();

        void stop();
        
        void transpose(Int32 semitones);
        
        void setPlayRate(Float64 rate);
        
        void loadFile(String const& path);
        
        void setFile(WaveFile* file);
        
        void setLooping(bool value);
        
        void setPlayHead(Float64 timeInSeconds);
        
        void setStartTime(Float64 timeInSeconds);
        
        void setEndTime(Float64 timeInSeconds);
        
        void setStartSample(size_t sample);
        
        void setEndSample(size_t sample);
        
        void resetStartAndEnd();
        
        void setReverse(bool value);
        
        NODISCARD WaveFile resample();
        
        NODISCARD FORCEINLINE bool reverses() const { return Math::deq(-1.0, direction); }
        
        NODISCARD FORCEINLINE bool looping() const { return this->shouldLoop; }
        
        NODISCARD FORCEINLINE Float64 playRate() const { return this->speed; }
        
        NODISCARD FORCEINLINE Float64 duration() const {
            
            if(!wavefile) return 0.0;
            
            return f64(wavefile->samplesPerChannel) / wavefile->sampleRate;
            
        }
        
    private:
        
        void finished();

        void resetPlayHead();

        void render(AudioBuffer<Float64> const& buffer, Float64 rate);
        
        WaveFile* wavefile = nullptr;
        
        bool playing = false;
        bool shouldLoop = false;

        Float64 direction = 1.0;
        Float64 speed = 1.0;
        Float64 playHead = 0;

        size_t startSample = 0;
        size_t endSample = 0;

    };

}
