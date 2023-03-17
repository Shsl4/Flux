#pragma once

#include <Nucleus/Nucleus.h>
#include <vector>

using namespace Nucleus;

class RtMidiIn;

namespace Flux {
    
    enum class MidiEvent {

        Invalid = 0,
        NoteDown = 144,
        NoteUp = 128,
        PitchBend = 224,
        CC = 176    
    
    };

    struct MidiMessage {
    
        MidiMessage(const MidiEvent event, const UInt dataByte, const UInt valueByte)
            : event(event),
              dataByte(dataByte),
              valueByte(valueByte) {}

        NODISCARD UInt cc() const;
        
        NODISCARD UInt noteNumber() const;

        NODISCARD Float64 noteFrequency() const;
        
        NODISCARD Float64 linearValue() const;
        
        NODISCARD String toString() const;

        MidiEvent event;
        UInt dataByte;
        UInt valueByte;
    
    };
    
    class MidiManager {
        
    public:
        
        MidiManager();
        
        void openDevice(UInt deviceIndex);

        void closeDevice() const;

        void listDevices() const;

        void simulateMessage(Int key, bool down);
        
        virtual void receiveMessage(MidiMessage const& message) = 0;

        virtual ~MidiManager();
        
    private:

        UInt simulationOctave = 5;
        UInt simulationVelocity = 100;
        
        void registerCommands();

        static MidiEvent asEvent(UInt8 value);

        static void eventCallback(Float64 deltaTime, std::vector<UInt8>* message, void* data);

        UInt currentDevice = 0;
        RtMidiIn* midiIn = nullptr;

    };


}

