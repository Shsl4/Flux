#pragma once

#include <Flux/Audio/Effects/Filters/Filter.h>

namespace Flux::Audio {
    
    class SAResonator : public Filter {

    public:

        SAResonator() = default;

        NODISCARD Float64 magnitude(Float64 omega) const override;

        NODISCARD Float64 argument(Float64 omega) const override;
        
    protected:
        
        Float64 processSingle(Float64 xn, size_t channel) override;

        void recalculateCoefficients() override;
        
    };
    
}

