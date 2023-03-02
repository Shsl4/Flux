#include "Application/FilterGraph.h"

namespace Flux{

    FilterGraph::FilterGraph(Point const& p, Point const& s): Component(p, s) {
        setColor(Color::fromHex(0x303030ff));
    }
    
    void FilterGraph::draw(SkCanvas *canvas, Float64 deltaTime) {

        Component::draw(canvas, deltaTime);
        drawGrid(canvas);

        SkPaint paint;
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(2.0f);
        paint.setColor(SK_ColorWHITE);

        canvas->drawPath(path, paint);

    }

    void FilterGraph::addListener(FilterGraphListener* listener) {
        if(listener) {
            this->listeners += listener;
        }
    }
    
    void FilterGraph::drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        const Point pos = globalTransform().position;
        const Point scale = size();

        const Range xRange = { pos.x, pos.x + scale.x };
        const Range yRange = { pos.y, pos.y + scale.y };
        const Range resonanceRange = { 10.0f, 0.25f };
        const Range<Float32> linRange = Range<Float32>::makeLinearRange();

        t = Range<Float32>::translateValue(Math::clamp(f32(x), xRange.getMin(), xRange.getMax()), xRange, linRange);
        const Float32 r = Range<Float32>::translateValue(Math::clamp(f32(y), yRange.getMin(), yRange.getMax()), yRange, resonanceRange);
        f = Math::easeIn(10.0f, 10050.0f, t, 5.0f);

        for (auto* listener : listeners) {
            listener->onValueChange(f64(f), f64(r));
        }

        recalculatePath();

    }

    void FilterGraph::recalculatePath() {

        path.reset();

        const Point pos = globalTransform().position;
        Point lastPoint = pos;
        lastPoint.y += size().y;

        constexpr Float64 sr = 44100;
        constexpr Float64 nyquist = sr / 2.0;
        constexpr Float64 pi = Math::pi<Float64>;
        constexpr UInt points = 500;
        constexpr Float64 mindB = -20.0;
        constexpr Float64 maxdB = 20.0;

        const auto values = distributeAround(f32(f), nyquist, points);

        for (UInt i = 0; i < points; ++i) {

            const Float64 mag = filterRef->getMagnitude(values[i]);
            const Float64 response = 20.0 * log10(mag);
            const Float64 freq = (values[i] / pi) * nyquist;
            const Float64 finalResponse = Math::clamp(response, mindB, maxdB);
            const Range<Float64> logRange = { log10(10), log10(nyquist) };
            const Range<Float64> linRange = Range<Float64>::makeLinearRange();

            const Float64 normalizedResponse = (finalResponse - mindB) / (maxdB - mindB);
            const Float64 normalizedFrequency = Range<Float64>::translateValue(log10(freq), logRange, linRange);

            if(!isfinite(normalizedResponse)) continue;

            const Float32 drawX = pos.x + f32(normalizedFrequency) * size().x;
            const Float32 drawY = pos.y + size().y - f32(normalizedResponse) * size().y;

            Point newPoint = { drawX, drawY };

            if(i == 0){
                path.moveTo(newPoint.x, newPoint.y);
            }
            else{

                path.moveTo(lastPoint.x, lastPoint.y);

                if(Math::dneq(finalResponse, mindB))
                    path.lineTo(newPoint.x, newPoint.y);
            }

            lastPoint = newPoint;

        }


    }

    Float64 FilterGraph::freqToRad(Float64 f, Float64 ny) {
        return (f * Math::pi<Float64>) / ny;
    }

    MutableArray<Float64> FilterGraph::distributeAround(Float64 freq, Float64 ny, UInt count) {

        auto values = MutableArray<Float64>(count);
        const Float64 baseRad = freqToRad(10.0, ny);

        // todo: distribute around f
        for (size_t i = 0; i < count; ++i) {
            values += baseRad + (Math::pi<Float64> - baseRad) * pow(f64(i) / f64(count), 5.0);
        }

        return values;

    }

    void FilterGraph::drawGrid(SkCanvas *canvas) const {

        SkPaint paint;
        paint.setStrokeWidth(2.5);
        paint.setColor(Color::fromHex(0x454545ff).skColor());

        const Range<Float32> logRange = { log10(10.0f), log10(44100.0f / 2.0f) };
        const Range<Float32> linRange = Range<Float32>::makeLinearRange();
        const Point position = globalTransform().position;
        const Point scale = size();

        canvas->drawLine(position.x, position.y + scale.y / 2.0f, position.x + scale.x, position.y + scale.y / 2.0f, paint);
        paint.setStrokeWidth(1.0);

        for (size_t p = 1; p <= 4; ++p) {
            
            const Float32 pw = std::pow(10.0f, f32(p));

            for (size_t d = 1; d <= 9; ++d) {
                
                const Float32 freq = pw * f32(d);
                const Float32 drawX = Range<Float32>::translateValue(log10(freq), logRange, linRange) * scale.x + position.x;

                if(drawX > position.x + scale.x) return;

                canvas->drawLine(drawX, position.y, drawX, position.y + scale.y, paint);

            }

        }

    }

}