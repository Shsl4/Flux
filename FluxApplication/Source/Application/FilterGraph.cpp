#include "Application/FilterGraph.h"

namespace Flux{

    void FilterGraph::initialize() {

        Component::initialize();
        setScale({ 200, 100 });
        setColor(UserInterface::LinearColor::fromHex(0x303030ff));

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
    
    void FilterGraph::onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        const SkVector pos = getAbsolutePosition();
        const SkVector scale = getScale();

        const Range xRange = { pos.fX, pos.fX + scale.fX };
        const Range yRange = { pos.fY, pos.fY + scale.fY };
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

        const SkVector pos = getAbsolutePosition();
        SkVector lastPoint = getAbsolutePosition();
        lastPoint.fY += getScale().fY;

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

            const Float32 drawX = pos.fX + f32(normalizedFrequency) * getScale().fX;
            const Float32 drawY = pos.fY + getScale().fY - f32(normalizedResponse) * getScale().fY;

            SkVector newPoint = { drawX, drawY };

            if(i == 0){
                path.moveTo(newPoint);
            }
            else{

                path.moveTo(lastPoint);

                if(Math::dneq(finalResponse, mindB))
                    path.lineTo(newPoint);
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
        paint.setColor(UserInterface::LinearColor::fromHex(0x454545ff).toSkColor());

        const Range<Float32> logRange = { log10(10.0f), log10(44100.0f / 2.0f) };
        const Range<Float32> linRange = Range<Float32>::makeLinearRange();
        const SkVector position = getAbsolutePosition();
        const SkVector scale = getScale();

        canvas->drawLine(position.fX, position.fY + scale.fY / 2.0f, position.fX + scale.fX, position.fY + scale.fY / 2.0f, paint);
        paint.setStrokeWidth(1.0);

        for (size_t p = 1; p <= 4; ++p) {
            
            const Float32 pw = std::pow(10.0f, f32(p));

            for (size_t d = 1; d <= 9; ++d) {
                
                const Float32 freq = pw * f32(d);
                const Float32 drawX = Range<Float32>::translateValue(log10(freq), logRange, linRange) * scale.fX + position.fX;

                if(drawX > position.fX + scale.fX) return;

                canvas->drawLine(drawX, position.fY, drawX, position.fY + scale.fY, paint);

            }

        }

    }

}