#pragma once

#include <MIDI/MidiManager.h>

class RtAudio;

namespace Flux {

    class AudioDevice : public MidiManager {
        
    public:

        AudioDevice();
        
        ~AudioDevice() override;

        void listAudioDevices() const;
        
        void initialize(Float64 rate, UInt size);

        void setSampleRate(Float64 value);
        
        void setBufferSize(UInt value);

        void closeOutputDevice();

        void openOutputDevice(UInt deviceId);
        
        virtual void prepare(Float64 rate, UInt size) = 0;
        
        virtual void process(Float64* buffer) = 0;

        void registerAudioCommands();

        NODISCARD UInt numOutputChannels() const;
        
        NODISCARD UInt numInputChannels() const;

        NODISCARD Float64 sampleRate() const;

        NODISCARD UInt bufferSize() const;

    protected:

        virtual void outputDeviceOpened();
        
        virtual void outputDeviceClosed();
        
    private:

        RtAudio* audio = nullptr;
        Float64 sr = 0.0;
        UInt bufSize = 0;

        UInt inputChannels = 0;
        UInt outputChannels = 0;

        UInt outputDevice = 0;
        
    };
    
}


