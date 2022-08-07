#pragma once

#include <cstdint>
#include <iostream>

#ifndef _WIN64

#define FORCEINLINE __attribute__((always_inline)) inline

#else

#define FORCEINLINE __forceinline

#endif

#define NODISCARD [[nodiscard]]
#define UNUSED [[maybe_unused]]
#define NORETURN [[noreturn]]
#define DEPRECATED [[deprecated]]

namespace Flux {

    using Float32 = float;
    using Float64 = double;
    
    using Int = int32_t;
    using Int8 = char;
    using Int16 = int16_t;
    using Int32 = int32_t;
    using Int64 = int64_t;

    using UInt = uint32_t;
    using UInt8 = uint8_t;
    using UInt16 = uint16_t;
    using UInt32 = uint32_t;
    using UInt64 = uint64_t;
    
    void expectImpl(bool b, const char* f);
    NORETURN void abortImpl(const char* f);

    void setTerminationCallback(void(*callback)());
    
}

#define fassert(e) Flux::expectImpl((e), #e)
#define fassertf(e, f) Flux::expectImpl(e, f)
#define fabort(f) Flux::abortImpl(f)

#define f32(v) static_cast<Flux::Float32>(v)
#define f64(v) static_cast<Flux::Float64>(v)

