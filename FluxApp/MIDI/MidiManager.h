#pragma once

#include <Flux/Core/Flux.h>
#include <vector>

#include "Flux/Core/Memory/Allocator.h"
#include "Flux/Core/Memory/String.h"

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

        NODISCARD UInt getCC() const;
        
        NODISCARD UInt getNoteNumber() const;

        NODISCARD Float64 getNoteFrequency() const;
        
        NODISCARD Float64 getLinearValue() const;
        
        NODISCARD String toString() const;

        MidiEvent event;
        UInt dataByte;
        UInt valueByte;
    
    };
    
    class MidiManager {
        
    public:
        
        MidiManager();
        
        void openMidiDevice(UInt deviceIndex);

        void closeMidiDevice() const;

        void listMidiDevices() const;

        void simulateMidiMessage(Int key, bool down);
        
        virtual void onMidiMessage(MidiMessage const& message) = 0;

        virtual ~MidiManager();
        
    private:

        UInt simulationOctave = 5;
        UInt simulationVelocity = 100;
        
        void registerCommands();

        static MidiEvent asMIDIEvent(const UInt8 value);

        static void eventCallback(Float64 deltaTime, std::vector<UInt8>* message, void* data);

        UInt currentDevice = 0;
        RtMidiIn* midiIn = nullptr;
        Allocator<RtMidiIn> allocator;

    };


}

