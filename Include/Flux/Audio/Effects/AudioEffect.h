#pragma once

#include <Flux/Audio/AudioObject.h>

#include <Audio/AudioBuffer.h>

namespace Flux::Audio {

    class AudioEffect : public AudioObject {
        
    public:
                
        virtual void process(AudioBuffer<Float64> const& buffer) = 0;
        
        virtual Float64 processSingle(Float64 xn, size_t channel) { return 0.0; }
        
        constexpr static Float64 kSmallestPositiveFloatValue = 1.175494351e-38;
        constexpr static Float64 kSmallestNegativeFloatValue = -1.175494351e-38;
        
        static bool checkFloatUnderflow(Float64& value) {
            
            if ((value > 0.0 && value < kSmallestPositiveFloatValue) ||
                (value < 0.0 && value > kSmallestNegativeFloatValue)) {
                
                value = 0.0;
                return true;
                
                }

            return false;
            
        }
        
    };
    
}
