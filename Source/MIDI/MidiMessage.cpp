#include <MIDI/MidiMessage.h>

namespace Flux {

    String MidiMessage::toString() const {
        
        switch (event) {

        case MidiEvent::invalid:
            return "Invalid";
            
        case MidiEvent::noteDown:
            return String::format("Note {} Down | Velocity: {}", dataByte, valueByte);
            
        case MidiEvent::noteUp:
            return String::format("Note {} Up | Velocity: {}", dataByte, valueByte);
            
        case MidiEvent::pitchBend:
            return String::format("PitchBend | Value: {}", valueByte);
            
        case MidiEvent::cc:
            return String::format("CC {} | Value: {}", dataByte, valueByte);
            
        }
        
        return "Invalid";
        
    }
    
}
