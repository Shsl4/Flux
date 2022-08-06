#pragma once

#include <Flux/Audio/Effects/Filters/Filter.h>

namespace Flux::Audio {
    
    class Resonator : public Filter {
        
    public:

        Resonator() = default;
        
        Float64 getMagnitude(Float64 omega) const;
        
        Float64 getArgument(Float64 omega) const;

    protected:

        Float64 processSingle(Float64 xn) override;
        
        void recalculateCoefficients() override;
        
    };

}
