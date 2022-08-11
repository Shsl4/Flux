#pragma once

#include <Flux/Core/Flux.h>
#include <Flux/Core/Interfaces/Identifiable.h>

namespace Flux::Audio {
        
    class AudioObject : public Identifiable {

    public:
        
        virtual void prepare(Float64 rate, UInt size);
        
        FORCEINLINE Float64 getSampleRate() const { return sampleRate; }
        
        FORCEINLINE UInt getBufferSize() const { return bufferSize; }
        
        virtual ~AudioObject() = default;

    private:
        
        UInt bufferSize = 0;
        Float64 sampleRate = 0.0;
        
    };
    
}
