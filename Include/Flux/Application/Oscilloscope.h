#pragma once

#include <Application/BodePlot.h>
#include <Utility/Timer.h>

namespace Flux {

    class Oscilloscope : public Component {

    public:

        Oscilloscope(const Point &p, const Point &s);

        void draw(Graphics const& graphics) override;

        void initialize() override;

        void feed(AudioBuffer<Float64> const& buffer);

        void recalculatePath();

    private:

        Timer t;

        Path path;
        Path p;
        ColorScheme scheme = ColorScheme::coral;

        size_t windowSize = 512;
        MutableArray<Float64> data;

    };

}
