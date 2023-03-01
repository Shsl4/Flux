#pragma once

#include <Nucleus/Nucleus.h>
#include <skia/core/SkColor.h>

using namespace Nucleus;

namespace Flux::UI {

    struct LinearColor {

    private:

        LinearColor() = default;

    public:

        LinearColor(const Float32 r, const Float32 g, const Float32 b, const Float32 a) : r(r), g(g), b(b), a(a) {
            
        }
        
        static Float32 nthBit(const UInt32 number, const UInt32 n) {

            return static_cast<Float32>((number >> (8*n)) & 0xff);
        
        }

        LinearColor operator*(Float32 scale) const {
            return { this->r * scale, this->g * scale, this->b * scale, this->a * scale };
        }
        
        LinearColor operator+(LinearColor const &other) const {
            return { this->r + other.r, this->g + other.g, this->b + other.b, this->a + other.a };
        }

        LinearColor operator-(LinearColor const &other) const {
            return { this->r - other.r, this->g - other.g, this->b - other.b, this->a - other.a };

        }

        NODISCARD LinearColor lighter(const UInt8 value) const {

            LinearColor c = *this;

            c.r = Math::clamp( c.r + static_cast<Float32>(value) / 255.0f, 0.0f, 1.0f);
            c.g = Math::clamp( c.g + static_cast<Float32>(value) / 255.0f, 0.0f, 1.0f);
            c.b = Math::clamp( c.b + static_cast<Float32>(value) / 255.0f, 0.0f, 1.0f);

            return c;
            
        }
        
        NODISCARD LinearColor darker(const UInt8 value) const {

            LinearColor c = *this;

            c.r = Math::clamp( c.r - static_cast<Float32>(value) / 255.0f, 0.0f, 1.0f);
            c.g = Math::clamp( c.g - static_cast<Float32>(value) / 255.0f, 0.0f, 1.0f);
            c.b = Math::clamp( c.b - static_cast<Float32>(value) / 255.0f, 0.0f, 1.0f);

            return c;
            
        }
        
        static LinearColor fromHex(UInt32 hex) {

            return {
                
                nthBit(hex, 3) / 255.0f,
                nthBit(hex, 2) / 255.0f,
                nthBit(hex,  1) / 255.0f,
                nthBit(hex,  0) / 255.0f
                
            };

        }

        static LinearColor randomColor() {

            return {
                
                Random::randomFloat(0.0f, 1.0f),
                Random::randomFloat(0.0f, 1.0f),
                Random::randomFloat(0.0f, 1.0f),
                1.0f
                
            };        
            
        }
        
        NODISCARD SkColor4f skColor() { return *reinterpret_cast<SkColor4f*>(this); }
        
        NODISCARD bool transparent() const { return Math::feq(0.0f, a); }
        NODISCARD Float32 red() const { return r; }
        NODISCARD Float32 green() const { return g; }
        NODISCARD Float32 blue() const { return b; }
        NODISCARD Float32 alpha() const { return a; }

    private:

        Float32 r, g, b, a;

    };
    
    class Colors {

    public:
    
        static inline const LinearColor red = LinearColor::fromHex(0xff0000ff);
        static inline const LinearColor green = LinearColor::fromHex(0x00ff00ff);
        static inline const LinearColor blue = LinearColor::fromHex(0x0000ffff);
        static inline const LinearColor white = LinearColor::fromHex(0xffffffff);
        static inline const LinearColor black = LinearColor::fromHex(0x000000ff);
        static inline const LinearColor transparent = LinearColor::fromHex(0xffffff00);
    
    };

    
}
