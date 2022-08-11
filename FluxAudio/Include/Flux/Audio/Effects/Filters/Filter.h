#pragma once

#include <Flux/Audio/Effects/AudioEffect.h>
#include <complex>

namespace Flux::Audio {

    enum FilterCoefficients {
        a0 = 0, a1, a2, b1, b2, c1, c2, coeffCount
    };

    enum { x_z1, x_z2, y_z1, y_z2, stateCount };
    
    class Filter : public AudioEffect {
    
    public:
        
        void prepare(Float64 rate, UInt size) override;

        bool process(Float64* buffer) override;

        virtual void setCutoffFrequency(Float64 cutoff);
        virtual void setResonance(Float64 value);
        
        FORCEINLINE Float64 getCutoffFrequency() const { return cutoffFrequency; }
        FORCEINLINE Float64 getResonance() const { return resonance; }
        
        FORCEINLINE const Float64* getCoefficients() const { return this->coefficients; }
        FORCEINLINE const Float64* getStateArray() const { return this->stateArray; }
    
    protected:

        virtual void recalculateCoefficients() = 0;
                
        /**
         * \brief The filter coefficient array
         */
        Float64 coefficients[coeffCount] = {};

        /**
         * \brief The z-1 storage registers
         */
        Float64 stateArray[stateCount] = {};
        
    private:

        Float64 cutoffFrequency = 440.0;
        Float64 resonance = 1.0;
        
    };

    
}
