#pragma once

#include <Nucleus/Nucleus.h>

using namespace Nucleus;

namespace Flux {

    template<typename ValueType>
    class AudioBuffer {

    public:
        
        AudioBuffer(ValueType* buffer, const UInt channelCount, const UInt bufferSize)
            : channelCount(channelCount), size(bufferSize) { this->buffer = buffer; }
        

        NODISCARD FORCEINLINE ValueType* operator[](size_t channel) const {

            assert(channel < channelCount);
            return buffer + size * channel;
            
        }

        NODISCARD FORCEINLINE ValueType* data() const { return buffer; }
        
        NODISCARD FORCEINLINE UInt bufferSize() const { return size; }

        NODISCARD FORCEINLINE UInt channels() const { return channelCount; }
    
    private:

        const UInt channelCount;
        const UInt size;
        ValueType* buffer;

    };
    
}
