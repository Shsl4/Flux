#pragma once

#include <Flux/Core/Flux.h>
#include <Flux/UI/Components/Component.h>
#include <Flux/Audio/Effects/Filters/IIRFilter.h>
#include <skia/core/SkPath.h>

#include <utility>

namespace Flux{

    template<typename NumberType>
    struct Range{
        
        Range(NumberType min, NumberType max) : min(min), max(max) {}

        NODISCARD static Range makeLinearRange() { return { static_cast<NumberType>(0), static_cast<NumberType>(1) }; }

        NODISCARD FORCEINLINE NumberType getMin() const{ return this->min; }

        NODISCARD FORCEINLINE NumberType getMax() const{ return this->max; }

        static NumberType translateValue(NumberType value, Range<NumberType> fromRange, Range<NumberType> toRange){

            NumberType fromValue = fromRange.getMax() - fromRange.getMin();
            NumberType toValue = toRange.getMax() - toRange.getMin();
            return (value - fromRange.getMin()) * toValue / fromValue + toRange.getMin();

        }

    private:

        NumberType min;
        NumberType max;

    };

    class Bounce
    {
    public:

        static Float64 In (Float64 k) {
            return 1.0 - Out(1.0 - k);
        }

        static Float64 Out (Float64 k) {

            if (k < (1.0 / 2.75)) {
                return 7.5625 * k * k;
            }
            if (k < (2.0 / 2.75)) {
                const Float64 v = k - (1.5 / 2.75);
                return 7.5625 * v * v + 0.75;
            }
            if (k < (2.5 / 2.75)) {
                const Float64 v = k - (2.25 / 2.75);
                return 7.5625 * v * v + 0.9375;
            }
            
            const Float64 v = k - (2.625 / 2.75);
            return 7.5625 * v * v + 0.984375;
            
        }

        static Float64 InOut (Float64 k) {
            if (k < 0.5) return In(k * 2.0) * 0.5;
            return Out(k * 2.0 - 1.0) * 0.5 + 0.5;
        }
    };

    class FilterGraphListener {
        
    public:
        
        virtual ~FilterGraphListener() = default;

        virtual void onValueChange(Float64 frequency, Float64 resonance) = 0;
        
    };

    class FilterGraph : public UserInterface::Component {

    public:

        void initialize() override;

        static Float64 freqToRad(Float64 f, Float64 ny);

        static Array<Float64> distributeAround(Float64 freq, Float64 ny, UInt count);

        void drawGrid(SkCanvas* canvas) const;

        void recalculatePath();

        void onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

        void draw(SkCanvas *canvas, Float64 deltaTime) override;

        void addListener(FilterGraphListener* listener);

        const Audio::Filter* filterRef = nullptr;
        
    private:

        Float32 t = 0.0;
        Float32 f = 10.0;
        SkPath path;

        Array<FilterGraphListener*> listeners;
        
    };

}