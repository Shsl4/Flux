#pragma once

#include <Nucleus/Nucleus.h>

using namespace Nucleus;

namespace Flux::Audio {
        
    class AudioObject {

    public:
        
        virtual void prepare(Float64 rate, UInt size);
        
        NODISCARD FORCEINLINE Float64 getSampleRate() const { return sampleRate; }

        NODISCARD FORCEINLINE UInt getBufferSize() const { return bufferSize; }
        
        virtual ~AudioObject() = default;

    private:
        
        UInt bufferSize = 0;
        Float64 sampleRate = 0.0;
        
    };
    
}
