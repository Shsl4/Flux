#include <UI/Color.h>

namespace Flux {

    const ColorScheme ColorScheme::vividPink = ColorScheme::fromHex(0xFE5D9FFF);
    const ColorScheme ColorScheme::pastelGreen = ColorScheme::fromHex(0xC2F9BBFF);
    const ColorScheme ColorScheme::emerald = ColorScheme::fromHex(0x62C370FF);
    const ColorScheme ColorScheme::mint = ColorScheme::fromHex(0x09BC8AFF);
    const ColorScheme ColorScheme::coral = ColorScheme::fromHex(0xF08080FF);
    const ColorScheme ColorScheme::onyx = ColorScheme::fromHex(0x151515ff);
    const ColorScheme ColorScheme::violetDirt = ColorScheme::fromHex(0x2D2327FF);
    const ColorScheme ColorScheme::intenseIndigo = ColorScheme::fromHex(0x6E44FFFF);
    const ColorScheme ColorScheme::deepPurple = ColorScheme::fromHex(0x5a189aff);
    const ColorScheme ColorScheme::flatBlue = ColorScheme::fromHex(0x0496ffff);
    const ColorScheme ColorScheme::turquoise = ColorScheme::fromHex(0x80ffdbff);
    const ColorScheme ColorScheme::magmaRed = ColorScheme::fromHex(0xd90429ff);
    const ColorScheme ColorScheme::treeBrown = ColorScheme::fromHex(0x7f5539ff);
    const ColorScheme ColorScheme::lightOrange = ColorScheme::fromHex(0xf48c06ff);
    const ColorScheme ColorScheme::yellow = ColorScheme::fromHex(0xffea00ff);
    const ColorScheme ColorScheme::coolScheme = ColorScheme(Color::fromHex(0xb160f0ff), Colors::black, Color::fromHex(0x606060ff), Colors::black, Color::fromHex(0x202020ff));

    ColorScheme::ColorScheme(const Color &lightest, const Color &lighter, const Color &base, const Color &darker,
                             const Color &darkest) : lightest(lightest), lighter(lighter), base(base), darker(darker),
                                                     darkest(darkest) {}

    Color Color::randomColor() {

        return {
            Random::randomFloat(0.0f, 1.0f),
            Random::randomFloat(0.0f, 1.0f),
            Random::randomFloat(0.0f, 1.0f),
            1.0f
        };

    }

}