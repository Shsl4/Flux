#pragma once

#include <Nucleus/Nucleus.h>

using namespace Nucleus;

namespace Flux {

    template<typename ValueType>
    class AudioBuffer {

    public:
        
        AudioBuffer(ValueType* buffer, const size_t channelCount, const size_t bufferSize)
            : channelCount(channelCount), size(bufferSize) { this->buffer = buffer; }
        

        NODISCARD FORCEINLINE ValueType* operator[](size_t channel) const {

            assert(channel < channelCount);
            return buffer + size * channel;
            
        }

        NODISCARD FORCEINLINE ValueType* data() const { return buffer; }
        
        NODISCARD FORCEINLINE size_t bufferSize() const { return size; }

        NODISCARD FORCEINLINE size_t channels() const { return channelCount; }
    
    private:

        const size_t channelCount;
        const size_t size;
        ValueType* buffer;

    };
    
}
