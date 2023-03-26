#pragma once

#include <Flux/Audio/Effects/Filters/Filter.h>

namespace Flux::Audio {
    
    class Resonator : public Filter {
        
    public:

        Resonator() = default;

        NODISCARD Float64 magnitude(Float64 omega) const override;

        NODISCARD Float64 argument(Float64 omega) const override;

    protected:

        Float64 processSingle(Float64 xn, size_t channel) override;
        
        void recalculateCoefficients() override;
        
    };

}
