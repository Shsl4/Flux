#pragma once

#include <functional>

#include <Flux/Audio/Effects/Filters/Filter.h>

namespace Flux::Audio {
    
    class IIRFilter : public Filter {

    public:

        NODISCARD Float64 getMagnitude(Float64 omega) const override;
        
        NODISCARD Float64 getArgument(Float64 omega) const override;

        FORCEINLINE Float64 getMix() const { return this->mix; }

        bool process(Float64* buffer) override;

        void setMix(Float64 value);
        
    protected:
        
        Float64 processSingle(Float64 xn) override;

        void recalculateCoefficients() override = 0;

    private:

        Float64 mix = 1.0;
        
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
