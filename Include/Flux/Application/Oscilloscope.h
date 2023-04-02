#pragma once

#include <Application/BodePlot.h>
#include <Utility/Timer.h>

namespace Flux {

    class Oscilloscope : public Component {

    public:

        Oscilloscope(const Point &p, const Point &s);

        void draw(SkCanvas *canvas, Float64 deltaTime) override;

        void initialize() override;

        void feed(AudioBuffer<Float64> const& buffer);

        void recalculatePath();

    private:

        Timer t;

        SkPath path;
        SkPath p;
        SkPath pathToDraw;
        ColorScheme scheme = ColorScheme::coral;

        size_t windowSize = 512;
        MutableArray<Float64> data;

    };

}
