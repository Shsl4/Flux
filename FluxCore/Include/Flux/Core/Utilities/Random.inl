#pragma once

#ifndef __RAND_INCL
#error This file sould only be included from Random.h.
#endif

inline bool Flux::Random::randomBool() {
    return static_cast<bool>(generator() & 1);
}

template<typename NumberType>
NumberType Flux::Random::randomInteger(NumberType min, NumberType max) {
    static_assert(std::is_integral_v<NumberType>, "NumberType must be an integral number.");
    return generator() % (max - min + 1) + min;
}

template<typename NumberType>
NumberType Flux::Random::randomFloat(NumberType min, NumberType max) {
    static_assert(std::is_floating_point_v<NumberType>, "NumberType must be a floating point number.");
    NumberType scale = generator() / (NumberType) generator.max();
    return min + scale * (max - min);
}

template<typename NumberType>
Flux::Array<NumberType> Flux::Random::randomFloatArray(size_t size, NumberType min, NumberType max) {
    static_assert(std::is_floating_point_v<NumberType>, "NumberType must be a floating point number.");

    Array<NumberType> arr = Array<NumberType>(size);

    for (size_t i = 0; i < size; ++i) {
        arr.append(randomFloat(min, max));
    }

    return arr;

}

template<typename NumberType>
Flux::Array<NumberType> Flux::Random::randomIntegerArray(size_t size, NumberType min, NumberType max) {

    static_assert(std::is_integral_v<NumberType>, "NumberType must be an integral number.");

    Array<NumberType> arr = Array<NumberType>(size);

    for (size_t i = 0; i < size; ++i) {
        arr.append(randomInteger(min, max));
    }

    return arr;

}

inline std::minstd_rand Flux::Random::createGenerator() {

    std::minstd_rand gen;

    UInt32 seed = static_cast<UInt32>(std::chrono::system_clock::now().time_since_epoch().count());

    gen.seed(seed);

    return gen;

}
