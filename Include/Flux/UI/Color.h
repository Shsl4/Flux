#pragma once

#include <Nucleus/Nucleus.h>

using namespace Nucleus;

namespace Flux {

    struct Color;

    struct HSV{

         HSV(Float32 h, Float32 s, Float32 v) : h(Math::clampAround(h, 0.0f, 360.0f)),
            s(Math::clamp(s, 0.0f, 1.0f)), v(Math::clamp(v, 0.0f, 1.0f)) {

        }

        HSV withValue(Float32 value) const { HSV hsv = *this; hsv.v = Math::clamp(value, 0.0f, 1.0f); return hsv;}

        HSV withSaturation(Float32 value) const { HSV hsv = *this; hsv.s = Math::clamp(value, 0.0f, 1.0f); return hsv;}

        HSV withHue(Float32 value) const { HSV hsv = *this; hsv.h = Math::clamp(value, 0.0f, 360.0f); return hsv;}

        NODISCARD inline  Color rgb(Float32 alpha = 1.0f) const;

        Float32 h, s, v;

    };

    struct Color {

    private:

        Color() = default;

    public:

         Color(const Float32 r, const Float32 g, const Float32 b, const Float32 a) : r(r), g(g), b(b), a(a) {

        }
        
        static  Float32 nthBit(UInt32 number, UInt32 n) {
            return static_cast<Float32>((number >> (8*n)) & 0xff);
        }

        Color operator*(Float32 scale) const {
            return { this->r * scale, this->g * scale, this->b * scale, this->a * scale };
        }
        
        Color operator+(Color const &other) const {
            return { this->r + other.r, this->g + other.g, this->b + other.b, this->a + other.a };
        }

        Color operator-(Color const &other) const {
            return { this->r - other.r, this->g - other.g, this->b - other.b, this->a - other.a };

        }

        NODISCARD HSV hsv() const{

            const Float32 max = Math::max(r, g, b);
            const Float32 min = Math::min(r, g, b);
            const Float32 diff = max - min;
            Float32 h = -1.0f;

            if(Math::feq(max, min)) h = 0.0f;
            else if(Math::feq(max, r)) h = std::fmod(60.0f * ((g - b) / diff) + 360.0f, 360.0f);
            else if(Math::feq(max, g)) h = std::fmod(60.0f * ((b - r) / diff) + 120.0f, 360.0f);
            else if(Math::feq(max, b)) h = std::fmod(60.0f * ((r - g) / diff) + 240.0f, 360.0f);

            const Float32 s = Math::feq(max, 0.0f) ? 0.0f : (diff / max);

            return { h, s, max };

        }

         static Color fromHex(UInt32 hex){

            return {

                nthBit(hex, 3) / 255.0f,
                nthBit(hex, 2) / 255.0f,
                nthBit(hex, 1) / 255.0f,
                nthBit(hex, 0) / 255.0f

            };

        }

        static Color randomColor();

        NODISCARD bool transparent() const { return Math::feq(0.0f, a); }
        NODISCARD Float32 red() const { return r; }
        NODISCARD Float32 green() const { return g; }
        NODISCARD Float32 blue() const { return b; }
        NODISCARD Float32 alpha() const { return a; }

        static const Color white;
        static const Color black;
        static const Color tintPurple;
        static const Color tintBlue;
        static const Color tintOrange;
        static const Color tintGreen;
        static const Color tintCyan;
        static const Color tintPink;
        static const Color tintRed;
        static const Color tintYellow;
        static const Color tintTurquoise;
        static const Color darkGray;
        static const Color lightGray;
        static const Color lighterGray;
        static const Color clear;

    private:

        Float32 r, g, b, a;

    };

     Color Flux::HSV::rgb(Float32 alpha) const {

        Float32 c = v * s;
        Float32 x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        Float32 m = v - c;

        if (h <= 60.0f) return { c + m, x + m, m, alpha };
        if (h <= 120.0f) return { x + m, c + m, m, alpha };
        if (h <= 180.0f) return { m, c + m, x + m, alpha };
        if (h <= 240.0f) return { m, x + m, c + m, alpha };
        if (h <= 300.0f) return { x + m, m, c + m, alpha };

        return { c + m, m, x + m, alpha };

    }

    struct ColorScheme {

        ColorScheme() = default;

        ColorScheme(const Color &lightest, const Color &lighter, const Color &base, const Color &darker,
                    const Color &darkest);

        static ColorScheme fromHSV(HSV const& hsv){

            ColorScheme scheme;

            scheme.lightest = HSV(hsv.h, 0.6f * hsv.s, 1.4f * hsv.v).rgb();
            scheme.lighter = HSV(hsv.h, 0.8f * hsv.s, 1.2f * hsv.v).rgb();
            scheme.base = hsv.rgb();
            scheme.darker = HSV(hsv.h, 1.1f * hsv.s, 0.9f * hsv.v).rgb();
            scheme.darkest = HSV(hsv.h, 1.2f * hsv.s, 0.8f * hsv.v).rgb();

            return scheme;

        }

         static ColorScheme darkScheme(Color const& tint){

            return { tint, Color::black, Color::lighterGray, Color::black,
                               Color::darkGray };
            
         }

        FORCEINLINE static ColorScheme fromColor(Color const& color){

            return fromHSV(color.hsv());

        }

        FORCEINLINE static ColorScheme fromHex(UInt value){

            return fromColor(Color::fromHex(value));

        }

        FORCEINLINE static ColorScheme random(){

            return fromColor(Color::randomColor());

        }

        Color lightest = Color::black;
        Color lighter = Color::black;
        Color base = Color::black;
        Color darker = Color::black;
        Color darkest = Color::black;

        const static ColorScheme vividPink;
        const static ColorScheme pastelGreen;
        const static ColorScheme emerald;
        const static ColorScheme mint;
        const static ColorScheme coral;
        const static ColorScheme onyx;
        const static ColorScheme violetDirt;
        const static ColorScheme intenseIndigo;
        const static ColorScheme deepPurple;
        const static ColorScheme flatBlue;
        const static ColorScheme turquoise;
        const static ColorScheme magmaRed;
        const static ColorScheme treeBrown;
        const static ColorScheme lightOrange;
        const static ColorScheme yellow;

    };

}
