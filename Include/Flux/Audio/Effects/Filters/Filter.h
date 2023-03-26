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

        void process(AudioBuffer<Float64> const& buffer) override;
        
        NODISCARD virtual Float64 magnitude(Float64 omega) const;
        NODISCARD virtual Float64 argument(Float64 omega) const;
        
        virtual void setCutoffFrequency(Float64 cutoff);
        virtual void setResonance(Float64 value);
        
        NODISCARD FORCEINLINE Float64 cutoff() const { return cutoffFrequency; }
        NODISCARD FORCEINLINE Float64 resonance() const { return q; }
        NODISCARD FORCEINLINE size_t channelCount() const { return channels; }

    protected:

        virtual void recalculateCoefficients() = 0;
                
        /**
         * \brief The filter coefficient array
         */
        Float64 coefficients[coeffCount] = {};

        /**
         * \brief The z-1 storage registers
         */
        MutableArray<MutableArray<Float64>> state = {};
        
    private:

        size_t channels = 2;
        Float64 cutoffFrequency = 440.0;
        Float64 q = 1.0;
        
    };

    
}
