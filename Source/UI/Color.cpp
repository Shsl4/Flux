#include <UI/Color.h>

namespace Flux {

    const Color Color::white = Color::fromHex(0xffffffff);
    const Color Color::black = Color::fromHex(0x000000ff);
    const Color Color::tintPurple = Color::fromHex(0xb160f0ff);
    const Color Color::tintBlue = Color::fromHex(0x0090ffff);
    const Color Color::tintOrange = Color::fromHex(0xffae00ff);
    const Color Color::tintGreen = Color::fromHex(0x60f089ff);
    const Color Color::tintCyan = Color::fromHex(0x44e3fcff);
    const Color Color::tintPink = Color::fromHex(0xf060c0ff);
    const Color Color::tintRed = Color::fromHex(0xfc4444ff);
    const Color Color::tintYellow = Color::fromHex(0xfcf044ff);
    const Color Color::tintTurquoise = Color::fromHex(0x44fccbff);
    const Color Color::darkGray = Color::fromHex(0x202020ff);
    const Color Color::lightGray = Color::fromHex(0x252525ff);
    const Color Color::lighterGray = Color::fromHex(0x606060ff);
    const Color Color::clear = Color::fromHex(0xffffff00);

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