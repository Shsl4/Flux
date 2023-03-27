#pragma once

#include <Application/BodePlot.h>

namespace Flux {

    class Oscilloscope : public Component {

    public:

        Oscilloscope(const Point &p, const Point &s);

        void draw(SkCanvas *canvas, Float64 deltaTime) override;

        void initialize() override;

        void feed(AudioBuffer<Float64> const& buffer);

        void recalculatePath();

    private:

        SkPath path;
        SkPath pathToDraw;
        ColorScheme scheme = ColorScheme::coral;

        size_t windowSize = 8192 * 6;
        MutableArray<Float64> data;

    };

}