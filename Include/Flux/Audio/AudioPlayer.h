#pragma once

#include <Audio/AudioProcessor.h>
#include <Application/WaveFile.h>

namespace Flux {

    class AudioPlayer : public AudioProcessor {

    public:

        class Listener {

        public:

            virtual void fileLoaded(WaveFile* newFile) = 0;

            virtual ~Listener() = default;

        };

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

        void setReverse(bool value);

        void setAmplitude(Float64 dB);

        void resetStartAndEnd();

        void addListener(Listener* listener);

        void removeListener(Listener* listener);

        NODISCARD WaveFile resample();
        
        NODISCARD FORCEINLINE bool reverses() const { return Math::deq(-1.0, direction); }
        
        NODISCARD FORCEINLINE bool looping() const { return this->shouldLoop; }
        
        NODISCARD FORCEINLINE Float64 playRate() const { return this->speed; }

        NODISCARD FORCEINLINE Float64 endTime() const { return f64(endSample) / wavefile->sampleRate; }

        NODISCARD FORCEINLINE Float64 startTime() const { return f64(startSample) / wavefile->sampleRate; }
        
        NODISCARD FORCEINLINE Float64 duration() const {
            
            if(!wavefile) return 0.0;
            
            return f64(wavefile->samplesPerChannel) / wavefile->sampleRate;
            
        }

        NODISCARD FORCEINLINE Float64 progress() const {

            if(!wavefile) return 0.0;

            return playHead / f64(wavefile->samplesPerChannel);

        }

        NODISCARD FORCEINLINE Float64 currentTime() const {

            if(!wavefile) return 0.0;

            return progress() * duration();

        }

        NODISCARD FORCEINLINE WaveFile* file() const { return this->wavefile; }

        NODISCARD FORCEINLINE bool isPlaying() const { return playing; }

        NODISCARD FORCEINLINE Float64 amplitude() const { return amplitudeValue; }
        
    private:
        
        void finished();

        void resetPlayHead();

        void render(AudioBuffer<Float64> const& buffer, Float64 rate);
        
        WaveFile* wavefile = nullptr;
        
        bool playing = false;
        bool shouldLoop = false;

        Float64 direction = 1.0;
        Float64 speed = 1.0;
        Float64 playHead = 0.0;
        Float64 amplitudeValue = Audio::toAmplitude(-6.0);

        size_t startSample = 0;
        size_t endSample = 0;

        MutableArray<Listener*> listeners = {};

    };

}
