#include <Application/Oscilloscope.h>

namespace Flux {

    Oscilloscope::Oscilloscope(const Point &p, const Point &s) : Component(p, s) {
        setColor(scheme.darkest);
        data = MutableArray<Float64>::filled(windowSize);
    }

    void Oscilloscope::draw(Graphics const& graphics) {

        graphics.setColor(color());
        graphics.drawRect(globalTransform());

        graphics.setStyle(Graphics::Style::Stroke);
        graphics.setStrokeWidth(2.0f);
        graphics.setColor(scheme.lightest);
        graphics.drawPath(path);

        for (const auto& child : children()) {
            child->draw(graphics);
        }

    }

    void Oscilloscope::initialize() {
        Component::initialize();
        t.loop(1.0 / 60.0, [this] {
            recalculatePath();
        });
    }

    void Oscilloscope::feed(AudioBuffer<Float64> const& buffer) {

        const size_t bufferSize = buffer.bufferSize();

        memmove(data.data(), data.data() + bufferSize, sizeof(Float64) * (windowSize - bufferSize));
        memcpy(data.data() + (windowSize - bufferSize), buffer[0], sizeof(Float64) * bufferSize);

    }

    void Oscilloscope::recalculatePath() {

        p.reset();
        
        const Point pos = globalTransform().position;
        
        for(size_t i = 0; i < windowSize; ++i) {

            const Float64 value = (Math::clamp(data[i], -1.0, 1.0) + 1.0) / 2.0;
            const Float32 drawX = pos.x + size().x * (f32(i) / f32(windowSize));
            const Float32 drawY = pos.y + size().y * f32(value);

            const Point newPoint = { drawX, drawY };

            if(i == 0) {
                p.moveTo(newPoint.x, newPoint.y);
            }
            else{
                p.lineTo(newPoint.x, newPoint.y);
            }

        }

        path = p;

    }
    
}
