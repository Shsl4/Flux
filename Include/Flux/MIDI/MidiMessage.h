#pragma once

#include <Nucleus/Nucleus.h>

namespace Flux {

    using namespace Nucleus;

    enum class MidiEvent {

        invalid = 0,
        noteDown = 144,
        noteUp = 128,
        pitchBend = 224,
        cc = 176    
    
    };

    struct MidiMessage {
    
        MidiMessage(const MidiEvent event, const UInt dataByte, const UInt valueByte)
            : event(event),
              dataByte(dataByte),
              valueByte(valueByte) {}

        NODISCARD FORCEINLINE UInt cc() const { return this->dataByte; }
        
        NODISCARD FORCEINLINE UInt noteNumber() const { return this->dataByte; }

        NODISCARD FORCEINLINE Float64 noteFrequency() const {
            return 440.0 * std::pow(2.0, (f64(dataByte) - 69.0) / 12.0);
        }
        
        NODISCARD FORCEINLINE Float64 linearValue() const { return f64(valueByte) / 127.0; }
        
        NODISCARD FORCEINLINE String toString() const;

        NODISCARD FORCEINLINE MidiEvent type() const { return this->event; }

    private:
        
        MidiEvent event;
        UInt dataByte;
        UInt valueByte;
    
    };
    
    
}