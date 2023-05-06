#pragma once

#define SK_GL

#ifdef __APPLE__
#define SK_METAL
#elif defined(_WIN64)
#define SK_DIRECT3D
#define SK_VULKAN
#else
#define SK_VULKAN
#endif

#include <Nucleus/Nucleus.h>
#include <UI/Font.h>
#include <UI/Transform.h>
#include <UI/Color.h>
#include <UI/Path.h>

class SkPaint;
class SkCanvas;

namespace Flux {

    using namespace Nucleus;

    class Graphics {

    public:

        enum StrokeStyle {

            fill,
            stroke,
            strokeAndFill

        };

        Graphics();

        ~Graphics();

        void drawText(String const& text, Point const& position, Float32 fontSize, Font const& font) const;

        void drawRect(Point const& pos, Point const& size) const;

        void drawRect(Transform const& transform) const;

        void drawRoundedRect(Point const& pos, Point const& size, Float32 radius) const;

        void drawRoundedRect(Transform const& transform, Float32 radius) const;

        void drawLine(Point const& start, Point const& end) const;

        void drawArc(Point const& pos, Point const& size, Float32 startAngle, Float32 sweepAngle, bool center) const;

        void drawCircle(Point const& centerPos, Float32 radius) const;

        void setColor(Color const& color) const;

        void setAntiAliasing(bool value) const;

        void setStrokeStyle(StrokeStyle style) const;

        void setStrokeWidth(Float32 width) const;

        void drawPath(Path const& path) const;

        void save() const;

        void restore() const;

        void scale(Point const& s) const;

        void translate(Point const& offset) const;

        void clip(Point const& pos, Point const& size) const;

        void updateCanvas(SkCanvas* newCanvas);

        NODISCARD Float64 deltaTime() const { return this->dt; }

        void updateDeltaTime(Float64 deltaTime);

    private:

        void resetPaint() const;

        SkCanvas* canvas = nullptr;
        SkPaint* paint = nullptr;
        Float64 dt = 0.0;

    };

}