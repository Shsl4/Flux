#pragma once

#include <Nucleus/Nucleus.h>
#include <vector>
#include <MIDI/MidiReceiver.h>

class RtMidiIn;

namespace Flux {
    
    class MidiManager : public MidiReceiver {
        
    public:
        
        MidiManager();
        
        void openDevice(UInt deviceIndex);

        void closeDevice() const;

        void listDevices() const;

        void simulateMessage(Int key, bool down);
        
        ~MidiManager() override;
        
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

