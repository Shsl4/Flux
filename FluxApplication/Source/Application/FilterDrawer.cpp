#include "Application/FilterDrawer.h"

namespace Flux{

    void FilterDrawer::initialize() {

        Component::initialize();
        setScale({ 200, 100 });
        setColor(UserInterface::LinearColor::fromHex(0x303030ff));
        filter.prepare(44100, 256);
        filter.setCutoffFrequency(f);
        recalculatePath();

    }

    void Flux::FilterDrawer::draw(SkCanvas *canvas, Flux::Float64 deltaTime) {

        UserInterface::Component::draw(canvas, deltaTime);
        drawGrid(canvas);

        SkPaint paint;
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(2.0f);
        paint.setColor(SK_ColorWHITE);

        canvas->drawPath(path, paint);

    }

    void FilterDrawer::onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        SkVector pos = getAbsolutePosition();
        SkVector scale = getScale();

        Range<Float64> xRange = { pos.fX, pos.fX + scale.fX };
        Range<Float64> yRange = { pos.fY, pos.fY + scale.fY };
        Range<Float64> linRange = Range<Float64>::makeLinearRange();
        Range<Float64> resonanceRange = { 10.0, 0.25 };

        t = Range<Float64>::translateValue(Math::clamp(x, xRange.getMin(), xRange.getMax()), xRange, linRange);
        const Float64 r = Range<Float64>::translateValue(Math::clamp(y, yRange.getMin(), yRange.getMax()), yRange, resonanceRange);
        f = Math::easeIn(10.0, 44100.0 / 2.0, f32(t), 5);

        filter.setResonance(r);
        filter.setCutoffFrequency(f);

        recalculatePath();

    }

    void FilterDrawer::recalculatePath() {

        path.reset();

        const SkVector pos = getAbsolutePosition();
        SkVector lastPoint = getAbsolutePosition();
        lastPoint.fY += getScale().fY;

        constexpr Float64 sr = 44100;
        constexpr Float64 nyquist = sr / 2.0;
        constexpr Float64 pi = Math::pi<Float64>;
        constexpr UInt points = 1000;
        constexpr Float64 mindB = -20.0;
        constexpr Float64 maxdB = 20.0;

        auto values = distributeAround(f, nyquist, points);

        for (UInt i = 0; i < points; ++i) {

            const Float64 mag = filter.getMagnitude(values[i]);
            const Float64 response = 20.0 * log10(mag);
            const Float64 freq = (values[i] / pi) * nyquist;
            const Float64 finalResponse = Math::clamp(response, mindB, maxdB);
            const Range<Float64> logRange = { log10(10), log10(nyquist) };
            const Range<Float64> linRange = Range<Float64>::makeLinearRange();

            const Float64 normalizedResponse = ((finalResponse - mindB)) / (maxdB - mindB);
            const Float64 normalizedFrequency = Range<Float64>::translateValue(log10(freq), logRange, linRange);

            if(!isfinite(normalizedResponse)) continue;

            Float32 drawX = pos.fX + f32(normalizedFrequency * getScale().fX);
            Float32 drawY = pos.fY + getScale().fY - f32(normalizedResponse * getScale().fY);

            SkVector newPoint = { drawX, drawY };

            if(i == 0){
                path.moveTo(newPoint);
            }
            else{

                path.moveTo(lastPoint);

                if(finalResponse != mindB)
                    path.lineTo(newPoint);
            }

            lastPoint = newPoint;

        }


    }

    Float64 FilterDrawer::freqToRad(Float64 f, Float64 ny) {
        return (f * Math::pi<Float64>) / ny;
    }

    Array<Float64> FilterDrawer::distributeAround(Float64 freq, Float64 ny, UInt count) {

        Array<Float64> values = Array<Float64>(count);
        Float64 baseRad = freqToRad(10.0, ny);

        // todo: distribute around f
        for (size_t i = 0; i < count; ++i) {
            values += baseRad + (Math::pi<Float64> - baseRad) * pow(f64(i) / f64(count), 5.0);
        }

        return values;

    }

    void FilterDrawer::drawGrid(SkCanvas *canvas) {

        SkPaint paint;
        paint.setStrokeWidth(2.5);
        paint.setColor(UserInterface::LinearColor::fromHex(0x454545ff).toSkColor());

        const Range<Float64> logRange = { log10(10), log10(44100.0 / 2.0) };
        const Range<Float64> linRange = Range<Float64>::makeLinearRange();
        const SkVector position = getAbsolutePosition();
        const SkVector scale = getScale();

        canvas->drawLine(position.fX, position.fY + scale.fY / 2.0f, position.fX + scale.fX, position.fY + scale.fY / 2.0f, paint);
        paint.setStrokeWidth(1.0);

        for (size_t p = 1; p <= 4; ++p) {

            Float64 pw = std::pow(10.0, f64(p));

            for (size_t d = 1; d <= 9; ++d) {

                Float64 freq = pw * f64(d);

                const Float64 drawX = Range<Float64>::translateValue(log10(freq), logRange, linRange) * scale.fX + position.fX;

                if(drawX > position.fX + scale.fX) return;

                canvas->drawLine(f32(drawX), position.fY, f32(drawX), position.fY + scale.fY, paint);

            }

        }

    }

}