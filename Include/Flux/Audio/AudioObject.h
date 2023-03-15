#pragma once

#include <Nucleus/Nucleus.h>

using namespace Nucleus;

namespace Flux::Audio {
        
    class AudioObject {

    public:
        
        virtual void prepare(Float64 rate, UInt size);
        
        NODISCARD FORCEINLINE Float64 sampleRate() const { return sr; }

        NODISCARD FORCEINLINE UInt bufferSize() const { return sz; }
        
        virtual ~AudioObject() = default;

    private:
        
        UInt sz = 0;
        Float64 sr = 0.0;
        
    };
    
}