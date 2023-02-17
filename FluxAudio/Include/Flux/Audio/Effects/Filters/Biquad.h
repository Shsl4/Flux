#pragma once

#include <Flux/Audio/Effects/Filters/Filter.h>

namespace Flux::Audio {

    enum class BiquadAlgorithm { kDirect, kCanonical, kTransposeDirect, kTransposeCanonical };

    struct BiquadParameters {
        
        BiquadParameters() = default;

        BiquadAlgorithm calcType = BiquadAlgorithm::kDirect;
        
    };
    
    class Biquad : public Filter {
        
    public:

        Biquad();

        NODISCARD FORCEINLINE BiquadParameters getParameters() const { return this->params; }

        void setParameters(BiquadParameters const& value);
    
    protected:
        
        Float64 processSingle(Float64 xn) override;
        
        void recalculateCoefficients() override;
        
        BiquadParameters params;
    
    };
    
}
