#pragma once

#include <Flux/Core/Flux.h>

namespace Flux::Audio {
        
    class AudioObject {

    public:
        
        virtual void prepare(Float64 rate, UInt size);
        
        FORCEINLINE Float64 getSampleRate() const { return sampleRate; }
        
        FORCEINLINE UInt getBufferSize() const { return bufferSize; }

        virtual void process(Float64* buffer) = 0;
        
        virtual Float64 processSingle(Float64 xn);
        
        virtual ~AudioObject() = default;

    private:

        constexpr static Float64 kSmallestPositiveFloatValue = 1.175494351e-38;
        constexpr static Float64 kSmallestNegativeFloatValue = -1.175494351e-38;

        UInt bufferSize = 0;
        Float64 sampleRate = 0.0;

    public:

        static bool checkFloatUnderflow(Float64& value) {
            
            if (value > 0.0 && value < kSmallestPositiveFloatValue ||
                value < 0.0 && value > kSmallestNegativeFloatValue) {
                
                value = 0.0;
                return true;
                
                }

            return false;
            
        }
        
    };
    
}
