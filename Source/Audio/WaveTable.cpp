#include <Audio/WaveTable.h>

namespace Flux {

    WaveTable::WaveTable(WaveFile* file, size_t frameSize) {
        
        nthrowif(!file || file->channelCount == 0, "Creating wavetable with an invalid file.");

        if(file->channelCount > 1) {
            Console::log("{}[Warning]: Creating a WaveTable with an audio file that has multiple channels."
                        "Only the first channel will be used.{}", Console::yellow, Console::reset);
        }

        nthrowif(file->buffers[0].size() % frameSize != 0, "Trying to create a wavetable with a file that cannot be"
                                                            "split in even pieces with the specified frame size.");
        
        const size_t frameCount = file->buffers[0].size() / frameSize;

        this->frames = MutableArray<MutableArray<Float64>>(frameCount);

        for (size_t i = 0; i < frameCount; ++i) {

            frames += MutableArray<Float64>::filled(frameSize);
            memcpy(frames[i].data(), file->buffers[0].data() + i * frameSize, sizeof(Float64) * frameSize);
            
        }

        this->frameSize = frameSize;

        for (size_t i = 0; i < 16; ++i) {
            voices += Allocator<Voice>::construct();
        }

        this->currentFrame = 0;
        
    }

    void WaveTable::Voice::startPlaying(Float64 frequency, Float64 velocity) {

        this->playFrequency = frequency;
        this->velocity = Math::clamp(velocity, 0.0, 1.0);
        this->playHead = 0.0;
        
    }

    void WaveTable::Voice::stopPlaying() {
        this->velocity = 0.0;
    }

    WaveTable::WaveTable(WaveFile* file) : WaveTable(file, 2048) {
        
    }

    void WaveTable::process(AudioBuffer<Float64> const& buffer) {
        
        for(size_t sample = 0; sample < buffer.bufferSize(); ++sample){

            for(auto const& v : voices) {

                auto t = v->tick(frames[currentFrame].data(), f64(frameSize), sampleRate());
                
                for(size_t channel = 0; channel < buffer.channels(); ++channel) {
                
                    buffer[channel][sample] += t * 0.2;
                
                }
            
            }
            
        }
        
    }

    void WaveTable::startNote(MidiMessage const& message) {

        if(activeVoices.containsKey(message.noteNumber())) return;
            
        for(auto const& voice : voices) {
                
            if(!voice->playing()) {

                voice->startPlaying(message.noteFrequency(), message.linearValue());
                activeVoices.add({ message.noteNumber(), voice });
                break;
                    
            }
                
        }
            
    }

    void WaveTable::stopNote(MidiMessage const& message) {

        if(!activeVoices.containsKey(message.noteNumber())) return;
            
        activeVoices[message.noteNumber()]->stopPlaying();
        activeVoices.removeByKey(message.noteNumber());
            
    }

    void WaveTable::stopAllNotes() {

        for(auto const& voice : voices) {
                
            voice->stopPlaying();
                
        }

        activeVoices = {};
        
    }

    void WaveTable::setCurrentFrame(size_t frame) {

        assert(frame < maxFrames());
        this->currentFrame = frame;
        
    }
    
}
