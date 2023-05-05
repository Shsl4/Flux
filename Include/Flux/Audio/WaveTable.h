#pragma once

#include <Audio/AudioProcessor.h>
#include <Application/WaveFile.h>
#include <MIDI/MidiManager.h>
#include <Audio/ADSR.h>

namespace Flux {

    class WaveTable : public AudioProcessor {
        
    public:

    class Voice : public Audio::AudioObject {

        public:
            
            NODISCARD FORCEINLINE Float64 tick(const Float64* readBuffer) {
                
                const auto index = st(floor(playHead));
                size_t nextIndex = index + 1;

                if(nextIndex > st(bufferSize())) nextIndex = 0;

                const Float64 frac = playHead - f64(index);
                const Float64 nextFrac = 1.0 - frac;
        
                playHead += bufferSize() * playFrequency / sampleRate();
                if(playHead >= bufferSize()) { playHead -= bufferSize(); }
        
                return velocity * readBuffer[index] * envelope.tick(); //(frac * readBuffer[index] + nextFrac * readBuffer[nextIndex]);
                
            }

            void prepare(Float64 rate, UInt size) override;

            void startPlaying(Float64 frequency, Float64 velocity);
            
            void stopPlaying();

            NODISCARD FORCEINLINE bool playing() const { return !envelope.idle(); }
        
        private:

            ADSR envelope;
            Float64 playFrequency = 0.0;
            Float64 playHead = 0.0;
            Float64 velocity = 0.0;
            
        };

        explicit WaveTable(WaveFile* file);

        WaveTable(WaveFile* file, size_t frameSize);

        void prepare(Float64 rate, UInt size) override;

        void process(AudioBuffer<Float64> const& buffer) override;

        void startNote(MidiMessage const& message);

        void stopNote(MidiMessage const& message);

        void stopAllNotes();

        void setCurrentFrame(size_t frame);

        NODISCARD FORCEINLINE size_t maxFrames() const{ return frames.size(); }

    private:

        MutableArray<MutableArray<Float64>> frames;
        SmartArray<Voice> voices = {};
        Map<UInt, Voice*> activeVoices = {};
        size_t frameSize = 0;
        size_t currentFrame = 0;

    };
}
