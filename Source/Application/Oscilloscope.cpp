#include <Application/Oscilloscope.h>
#include "Audio/Engine.h"

namespace Flux {

    Oscilloscope::Oscilloscope(const Point &p, const Point &s) : Component(p, s) {
        setColor(scheme.darkest);
        data = MutableArray<Float64>::filled(windowSize);
    }

    void Oscilloscope::draw(SkCanvas *canvas, Float64 deltaTime) {

        SkPaint paint;
        const auto t = globalTransform();
        const SkRect rect = SkRect::MakeXYWH(t.position.x, t.position.y, t.size.x, t.size.y);

        paint.setColor(color().skColor());
        canvas->drawRect(rect, paint);

        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(2.0f);
        paint.setColor(scheme.lightest.skColor());

        recalculatePath();

        canvas->drawPath(path, paint);

        for (const auto& child : children()) {
            child->draw(canvas, deltaTime);
        }

    }

    void Oscilloscope::initialize() {
        Component::initialize();
    }

    void Oscilloscope::feed(AudioBuffer<Float64> const& buffer) {

        const size_t bufferSize = buffer.bufferSize();

        memcpy(data.data(), data.data() + bufferSize, sizeof(Float64) * (windowSize - bufferSize));
        memcpy(data.data() + (windowSize - bufferSize), buffer[0], sizeof(Float64) * bufferSize);

    }

    void Oscilloscope::recalculatePath() {

        path.reset();

        const Point pos = globalTransform().position;

        // TODO: Find a way to stabilize the drawing
        for(size_t i = 0; i < windowSize; i += 15) {

            const Float64 value = (Math::clamp(data[i], -1.0, 1.0) + 1.0) / 2.0;
            const Float32 drawX = pos.x + size().x * (f32(i) / f32(windowSize));
            const Float32 drawY = pos.y * 1.2f + size().y * f32(value) * 0.6f;

            Point newPoint = { drawX, drawY };

            if(i == 0) {
                path.moveTo(newPoint.x, newPoint.y);
            }
            else{
                path.lineTo(newPoint.x, newPoint.y);
            }

        }

    }


}
