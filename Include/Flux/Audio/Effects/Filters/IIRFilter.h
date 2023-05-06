#pragma once

#include <functional>

#include <Flux/Audio/Effects/Filters/Filter.h>

namespace Flux::Audio {
    
    class IIRFilter : public Filter {

    public:

        NODISCARD Float64 magnitude(Float64 omega) const override;
        
        NODISCARD Float64 argument(Float64 omega) const override;

        NODISCARD std::complex<Float64> openTransfer(Float64 omega) const override;

        void process(AudioBuffer<Float64> const& buffer) override;
    
    protected:
        
        Float64 processSingle(Float64 xn, size_t channel) override;

        void recalculateCoefficients() override = 0;
        
    };

    class LowPassFilter : public IIRFilter {

    protected:

        void recalculateCoefficients() override;

    };

    class HighPassFilter : public IIRFilter {

    protected:

        void recalculateCoefficients() override;

    };

    class BandPassFilter : public IIRFilter {

    protected:

        void recalculateCoefficients() override;

    };

    class NotchFilter : public IIRFilter {

    protected:

        void recalculateCoefficients() override;

    };

    class BWLowPassFilter : public IIRFilter {

    protected:

        void recalculateCoefficients() override;

    };
    
    class BWHighPassFilter : public IIRFilter {

    protected:

        void recalculateCoefficients() override;

    };
    
    class BWBandPassFilter : public IIRFilter {

    protected:

        void recalculateCoefficients() override;

    };

    class BWNotchFilter : public IIRFilter {

    protected:

        void recalculateCoefficients() override;

    };

    class AllpassFilter : public IIRFilter {

    protected:

        void recalculateCoefficients() override;

    };
    
}
