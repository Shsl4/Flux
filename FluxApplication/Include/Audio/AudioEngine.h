#pragma once

#include <MIDI/MidiManager.h>

class RtAudio;

namespace Flux {

    class AudioEngine {
        
    public:

        AudioEngine();
        
        virtual ~AudioEngine();

        void listDevices() const;
        
        void initialize(Float64 rate, UInt size);

        void setSampleRate(Float64 value);
        
        void setBufferSize(UInt value);

        bool open(UInt inputId, UInt outputId);

        void close();

        virtual void prepare(Float64 rate, UInt size) = 0;
        
        virtual void process(Float64* inputBuffer, Float64* outputBuffer) = 0;

        NODISCARD UInt numOutputChannels() const;
        
        NODISCARD UInt numInputChannels() const;

        NODISCARD Float64 sampleRate() const;

        NODISCARD UInt bufferSize() const;

    protected:

        virtual void opened();
        
        virtual void closed();
        
    private:

        RtAudio* audio = nullptr;
        Float64 sr = 0.0;

        UInt bufSize = 0;
        UInt inputChannels = 0;
        UInt outputChannels = 0;

    };
    
}


