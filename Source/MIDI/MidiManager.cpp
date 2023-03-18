#include <MIDI/MidiManager.h>
#include <rtmidi/RtMidi.h>

namespace Flux {
    
    void MidiManager::eventCallback(Float64 deltaTime, std::vector<UInt8>* message, void* data) {
        
        const MidiEvent event = asEvent(message->at(0));
        const UInt noteNumber = message->at(1);
        const UInt noteVelocity = message->at(2);

        const MidiMessage midiMessage = { event, noteNumber, noteVelocity };
        
        auto* manager = static_cast<MidiManager*>(data);

        manager->receiveMessage(midiMessage);
        
    }

    UInt MidiMessage::cc() const {
        return dataByte;
    }

    UInt MidiMessage::noteNumber() const {
        return dataByte;
    }

    Float64 MidiMessage::noteFrequency() const {

        return 440.0 * std::pow(2.0, (static_cast<Float64>(dataByte) - 69.0) / 12.0);
        
    }

    Float64 MidiMessage::linearValue() const {
        return static_cast<Float64>(valueByte) / 127.0;
    }

    String MidiMessage::toString() const {
        
        switch (event) {

        case MidiEvent::Invalid:
            return "Invalid";
            
        case MidiEvent::NoteDown:
            return String::format("Note {} Down | Velocity: {}", dataByte, valueByte);
            
        case MidiEvent::NoteUp:
            return String::format("Note {} Up | Velocity: {}", dataByte, valueByte);
            
        case MidiEvent::PitchBend:
            return String::format("PitchBend | Value: {}", valueByte);
            
        case MidiEvent::CC:
            return String::format("CC {} | Value: {}", dataByte, valueByte);
            
        }
        
        return "Invalid";
        
    }

    MidiManager::MidiManager() {

        std::vector<RtMidiIn::Api> apis;
        RtMidiIn::getCompiledApi(apis);
        
        if (apis.empty() || (apis.size() == 1 && apis[0] == RtMidiIn::Api::RTMIDI_DUMMY)) {
            throw Exceptions::Exception("No Audio API is available. Make sure you compiled RtMidi with at least one API.");
        }
        
        try{
            this->midiIn = Allocator<RtMidiIn>::construct();
        }
        catch(std::exception const&){
            throw Exceptions::Exception("Failed to create MIDI Manager.");
        }
        
        midiIn->setCallback(&MidiManager::eventCallback, this);
        midiIn->ignoreTypes(false, false, false);

        registerCommands();
        
    }

    void MidiManager::openDevice(UInt deviceIndex) {

        closeDevice();

        if(midiIn->getPortCount() <= 0) return;

        try {

            midiIn->openPort(deviceIndex);
            String deviceName = midiIn->getPortName(deviceIndex);
            deviceName = deviceName.substring(0, deviceName.size() - 2);
            currentDevice = deviceIndex;
            Console::log("Now using MIDI device on port {} ({}).\n", deviceIndex, deviceName);
            
        }
        catch(Exceptions::Exception const&) {

        }
        
    }

    void MidiManager::closeDevice() const {

        midiIn->closePort();
        
    }

    void MidiManager::listDevices() const {

        const UInt count = midiIn->getPortCount();

        if(count == 0) {

            Console::log("There are no MIDI devices available.\n");
            return;
            
        }

        Console::log("Listing MIDI devices:\n");

        for (UInt index = 0; index < count; ++index) {

            String deviceName = midiIn->getPortName(index);
            deviceName = deviceName.substring(0, deviceName.size() - 2);

            if(midiIn->isPortOpen() && index == currentDevice) {
                Console::log("[{}] -> {} (Active)\n", index, deviceName);
            }
            else {
                Console::log("[{}] -> {}\n", index, deviceName);
            }
            
            
        }
        
    }

    void MidiManager::simulateMessage(Int key, bool down) {

        // C = 64, D = 83, E = 68, F = 70, G = 71, A = 72, B = 74
        // Cs = 87, Ds = 69, Fs = 84, Gs = 89, As = 85

        Int note;

        if(key == 90 && down) {

            if(simulationOctave > 0) {
                --simulationOctave;
            }

            return;
            
        }
        
        if(key == 88 && down) {

            if(simulationOctave < 10) {
                ++simulationOctave;
            }

            return;
            
        }

        switch (key) {
        case 65: note = 0; break;
        case 87: note = 1; break;
        case 83: note = 2; break;
        case 69: note = 3; break;
        case 68: note = 4; break;
        case 70: note = 5; break;
        case 84: note = 6; break;
        case 71: note = 7; break;
        case 89: note = 8; break;
        case 72: note = 9; break;
        case 85: note = 10; break;
        case 74: note = 11; break;
        case 75: note = 12; break;
        case 79: note = 13; break;
        case 76: note = 14; break;
        case 80: note = 15; break;
        case 59: note = 16; break;
        case 39: note = 17; break;
        default: return;
        }

        const UInt realNote = simulationOctave * 12 + note;

        const MidiMessage message = { down ? MidiEvent::NoteDown : MidiEvent::NoteUp, realNote, simulationVelocity};

        receiveMessage(message);
        
    }

    MidiManager::~MidiManager() {

        Allocator<RtMidiIn>::destroy(midiIn);
        
    }

    void MidiManager::registerCommands() {

        
    }

    MidiEvent MidiManager::asEvent(UInt8 value) {
    
        switch (value) {
        
        case 144: return MidiEvent::NoteDown;
        case 128: return MidiEvent::NoteUp;
        case 224: return MidiEvent::PitchBend;
        case 176: return MidiEvent::CC;
        default: return MidiEvent::Invalid;
        
        }
    
    }
    
}
