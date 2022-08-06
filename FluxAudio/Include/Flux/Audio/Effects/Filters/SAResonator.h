#pragma once

#include <Flux/Audio/Effects/Filters/Filter.h>

namespace Flux::Audio {
    
    class SAResonator : public Filter {

    public:

        SAResonator() = default;

        Float64 getMagnitude(Float64 omega) const;
        
        Float64 getArgument(Float64 omega) const;
        
    protected:
        
        Float64 processSingle(Float64 xn) override;

        void recalculateCoefficients() override;
        
    };
    
}

