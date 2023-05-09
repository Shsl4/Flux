#include <UI/Graphics.h>

#include <skia/include/core/SkColorSpace.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkPathEffect.h>

namespace Flux {

    Graphics::Graphics() {
        this->paint = Allocator<SkPaint>::construct();
    }

    Graphics::~Graphics() {
        Allocator<SkPaint>::destroy(this->paint);
    }

    void Graphics::drawText(String const& text, Point const& position, Float32 fontSize, Font const& font) const {

        font.skiaFont()->setSize(fontSize);
        canvas->drawSimpleText(text.cString(), text.size(), SkTextEncoding::kUTF8, position.x,
                               position.y, *font.skiaFont(), *paint);
        resetPaint();

    }

    void Graphics::setColor(const Color &color) const {
        this->paint->setColor(*reinterpret_cast<const SkColor4f*>(&color));
    }

    void Graphics::drawRect(const Point &pos, const Point &size) const {
        canvas->drawRect(SkRect::MakeXYWH(pos.x, pos.y, size.x, size.y), *paint);
        resetPaint();
    }

    void Graphics::drawRect(const Transform& t) const {
        this->drawRect(t.position, t.size);
    }

    void Graphics::setAntiAliasing(bool value) const {
        paint->setAntiAlias(value);
    }

    void Graphics::setStrokeStyle(Graphics::StrokeStyle style) const {

        static const Map<Graphics::StrokeStyle, SkPaint::Style> map = {
                { Graphics::StrokeStyle::fill, SkPaint::Style::kFill_Style },
                { Graphics::StrokeStyle::stroke, SkPaint::Style::kStroke_Style },
                { Graphics::StrokeStyle::strokeAndFill, SkPaint::Style::kStrokeAndFill_Style }
        };

        paint->setStyle(map[style]);

    }

    void Graphics::setStrokeCap(StrokeCap style) const {
        
        static const Map<Graphics::StrokeCap, SkPaint::Cap> map = {
            { Graphics::StrokeCap::none, SkPaint::Cap::kButt_Cap },
            { Graphics::StrokeCap::round, SkPaint::Cap::kRound_Cap },
            { Graphics::StrokeCap::square, SkPaint::Cap::kSquare_Cap }
        };
        
        paint->setStrokeCap(map[style]);
        
    }

    void Graphics::setStrokeWidth(Float32 width) const {
        paint->setStrokeWidth(width);
    }

    void Graphics::drawPath(const Path &path) const {
        canvas->drawPath(*path.skPath(), *paint);
        resetPaint();
    }

    void Graphics::drawLine(const Point &start, const Point &end) const {
        canvas->drawLine(start.x, start.y, end.x, end.y, *paint);
    }

    void Graphics::drawArc(const Point &pos, const Point &size, Float32 startAngle, Float32 sweepAngle, bool center) const {

        const auto rect = SkRect::MakeXYWH(pos.x, pos.y, size.x, size.y);
        canvas->drawArc(rect, startAngle, sweepAngle, center, *paint);
        resetPaint();

    }

    void Graphics::drawCircle(const Point &centerPos, Float32 radius) const {
        canvas->drawCircle(centerPos.x, centerPos.y, radius, *paint);
        resetPaint();
    }

    void Graphics::save() const {
        canvas->save();
    }

    void Graphics::restore() const {
        canvas->restore();
    }

    void Graphics::scale(const Point &s) const {
        canvas->scale(s.x, s.y);
    }

    void Graphics::translate(const Point &offset) const {
        canvas->translate(offset.x, offset.y);
    }

    void Graphics::clip(const Point &pos, const Point &size) const {
        canvas->clipRect(SkRect::MakeXYWH(pos.x, pos.y, size.x, size.y));
    }

    void Graphics::drawRoundedRect(const Point &pos, const Point &size, Float32 radius) const {
        canvas->drawRoundRect(SkRect::MakeXYWH(pos.x, pos.y, size.x, size.y), radius, radius, *paint);
        resetPaint();
    }

    void Graphics::drawRoundedRect(const Transform &transform, Float32 radius) const {
        this->drawRoundedRect(transform.position, transform.size, radius);
    }

    void Graphics::updateCanvas(SkCanvas *newCanvas) {

        nthrowif(!newCanvas, "Trying to use an invalid canvas!");

        this->canvas = newCanvas;

    }

    void Graphics::updateDeltaTime(Float64 deltaTime) {
        this->dt = deltaTime;
    }

    void Graphics::resetPaint() const {
        paint->reset();
    }

}