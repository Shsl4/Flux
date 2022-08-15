#pragma once

#include <Flux/Core/Flux.h>

namespace Flux {

    template<typename ValueType>
    class AudioBuffer : public Identifiable {

    public:
        
        AudioBuffer(ValueType** buffer, const UInt channelCount, const UInt bufferSize)
            : channels(channelCount), bufferSize(bufferSize) { this->buffer = buffer; }
        
        NODISCARD FORCEINLINE ValueType* getChannelBuffer(const UInt channel) const {

            fassert(channel < channels);
            return buffer[channel];
            
        }

        NODISCARD FORCEINLINE ValueType** getRawBuffers() const { return buffer; }
        
        NODISCARD FORCEINLINE UInt getBufferSize() const { return bufferSize; }

        NODISCARD FORCEINLINE UInt getChannelCount() const { return channels; }
    
    private:

        const UInt channels;
        const UInt bufferSize;
        ValueType** buffer;

    };
    
}
