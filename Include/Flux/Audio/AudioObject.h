#pragma once

#include <Audio/AudioBuffer.h>

namespace Flux::Audio {

    NODISCARD FORCEINLINE Float64 toDecibels(Float64 amp){
        return 20.0 * std::log10(amp);
    }

    NODISCARD FORCEINLINE Float64 toAmplitude(Float64 dB){
        return std::pow(10.0, dB / 20.0);
    }
        
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
