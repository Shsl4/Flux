#pragma once

#include <MIDI/MidiManager.h>

class RtAudio;

namespace Flux {

    class AudioDevice : public MidiManager {
        
    public:

        AudioDevice();
        
        virtual ~AudioDevice();

        void listAudioDevices() const;
        
        void initialize(Float64 rate, UInt size);

        void setSampleRate(Float64 value);
        
        void setBufferSize(UInt value);

        void closeOutputDevice();

        void openAudioOutputDevice(UInt deviceId);
        
        virtual void prepare(Float64 rate, UInt size) = 0;
        
        virtual void process(Float64* buffer) = 0;

        void registerAudioCommands();

        NODISCARD UInt getOutputChannelCount() const;
        
        NODISCARD UInt getInputChannelCount() const;

        NODISCARD Float64 getSampleRate() const;

        NODISCARD UInt getBufferSize() const;

    protected:

        virtual void onOutputDeviceOpened();
        
        virtual void onOutputDeviceClosed();
        
    private:

        RtAudio* audio = nullptr;
        Float64 sampleRate = 0.0;
        UInt bufferSize = 0;

        UInt inputChannelCount = 0;
        UInt outputChannelCount = 0;

        UInt currentOutputDevice = 0;
        
    };
    
}


