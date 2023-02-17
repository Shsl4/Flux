#pragma once

#include <Flux/Audio/Effects/Filters/Filter.h>

namespace Flux::Audio {
    
    class SAResonator : public Filter {

    public:

        SAResonator() = default;

        NODISCARD Float64 getMagnitude(Float64 omega) const override;

        NODISCARD Float64 getArgument(Float64 omega) const override;
        
    protected:
        
        Float64 processSingle(Float64 xn) override;

        void recalculateCoefficients() override;
        
    };
    
}

