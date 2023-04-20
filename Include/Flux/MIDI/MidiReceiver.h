#pragma once

#include <MIDI/MidiMessage.h>

namespace Flux {

    class MidiReceiver {

    public:

        virtual void receiveMessage(MidiMessage const& message) = 0;

        virtual ~MidiReceiver() = default;
        
    };
    
}
