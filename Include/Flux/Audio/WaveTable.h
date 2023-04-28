#pragma once

#include <Audio/AudioObject.h>
#include <Application/WaveFile.h>
#include <MIDI/MidiManager.h>

namespace Flux {

    class WaveTable : public Audio::AudioObject {
        
    public:

        class Voice {

        public:
            
            NODISCARD FORCEINLINE Float64 tick(const Float64* readBuffer, Float64 size, Float64 sampleRate) {
                
                const auto index = st(floor(playHead));
                size_t nextIndex = index + 1;

                if(nextIndex > st(size)) nextIndex = 0;

                const Float64 frac = playHead - f64(index);
                const Float64 nextFrac = 1.0 - frac;
        
                playHead += size * playFrequency / sampleRate;
                if(playHead >= size) { playHead -= size; }
        
                return velocity * readBuffer[index]; //(frac * readBuffer[index] + nextFrac * readBuffer[nextIndex]);
                
            }

            void startPlaying(Float64 frequency, Float64 velocity);
            
            void stopPlaying();

            NODISCARD bool playing() const { return velocity > 0.0; }
        
        private:

            Float64 playFrequency = 0.0;
            Float64 playHead = 0.0;
            Float64 velocity = 0.0;
            
        };

        explicit WaveTable(WaveFile* file);

        WaveTable(WaveFile* file, size_t frameSize);
        
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
