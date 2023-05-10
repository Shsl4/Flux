#pragma once

#include <Nucleus/Nucleus.h>

namespace Flux {

    using namespace Nucleus;

    template<typename NumberType>
    struct Range{

        Range(NumberType min, NumberType max) : minValue(min), maxValue(max) {}

        NODISCARD FORCEINLINE NumberType min() const{ return this->minValue; }

        NODISCARD FORCEINLINE NumberType max() const{ return this->maxValue; }

        NODISCARD FORCEINLINE NumberType clamp(NumberType v) const { return Math::clamp(v, minValue, maxValue); }

        NODISCARD FORCEINLINE NumberType clampAround(NumberType v) const { return Math::clampAround(v, minValue, maxValue); }

        NODISCARD static NumberType translate(NumberType value, Range const& fromRange, Range const& toRange) {

            NumberType fromValue = fromRange.max() - fromRange.min();
            NumberType toValue = toRange.max() - toRange.min();
            return (value - fromRange.min()) * toValue / fromValue + toRange.min();

        }

        NODISCARD NumberType translateTo(NumberType value, Range const& toRange) const {

            NumberType fromValue = this->max() - this->min();
            NumberType toValue = toRange.max() - toRange.min();
            return (value - this->min()) * toValue / fromValue + toRange.min();

        }

        static const Range linear;

    private:

        NumberType minValue;
        NumberType maxValue;

    };

    template<typename NumberType>
    inline const Range<NumberType> Range<NumberType>::linear = { static_cast<NumberType>(0), static_cast<NumberType>(1) };

}