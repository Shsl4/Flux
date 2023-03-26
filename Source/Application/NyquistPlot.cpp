#include <Application/NyquistPlot.h>

namespace Flux {
    
    NyquistPlot::NyquistPlot(Point const& p, Point const& s): Component(p, s) {
        setColor(scheme.darkest);
        textSize = s.x * 0.025f;
    }
    
    void NyquistPlot::draw(SkCanvas* canvas, Float64 deltaTime) {

        SkPaint paint;
        const auto t = globalTransform();
        const SkRect rect = SkRect::MakeXYWH(t.position.x, t.position.y, t.size.x, t.size.y);

        paint.setColor(color().skColor());
        canvas->drawRect(rect, paint);
        
        drawCircle(canvas, deltaTime);

        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(2.0f);
        paint.setColor(scheme.lightest.skColor());

        canvas->save();
        canvas->clipRect(rect);
        
        canvas->drawPath(path, paint);

        for (const auto& child : children()) {
            child->draw(canvas, deltaTime);
        }

        canvas->restore();
        
    }
    
    void NyquistPlot::drawCircle(SkCanvas* canvas, Float64 deltaTime) const {

        SkPaint paint;
        paint.setStrokeWidth(2.0f);
        paint.setStyle(SkPaint::Style::kStroke_Style);
        paint.setColor(scheme.base.skColor());

        const Point position = globalTransform().position;
        const Point scale = size();
        
        canvas->drawCircle(position.x + scale.x / 2.0f, position.y + scale.y / 2.0f, scale.x / 4.0f, paint);

        paint.setStrokeWidth(1.0f);
        
        canvas->drawLine(position.x + scale.x / 2.0f, position.y, position.x + scale.x / 2.0f, position.y + scale.y, paint);
        canvas->drawLine(position.x, position.y + scale.y / 2.0f, position.x + scale.x, position.y + scale.y / 2.0f, paint);
        
    }
    
    void NyquistPlot::initialize() {

        this->reText = Factory::createComponent<Text>(Point(size().x - 100.0f, size().y / 2.0f - 100.0f),
            Point(100, 100), "Re", textSize, VAlignment::bottom, HAlignment::right);
        
        reText->setReactive(false);
        reText->setColor(scheme.lightest);

        this->imText = Factory::createComponent<Text>(Point(size().x / 2.0f, 0.0f),
            Point(100, 100), "Im", textSize, VAlignment::top, HAlignment::left);
        
        imText->setReactive(false);
        imText->setColor(scheme.lightest);
        
        addChild(reText);
        addChild(imText);
        
    }
    
    void NyquistPlot::setFilter(Audio::Filter* fil) {
        this->filter = fil;
        recalculateResponse();
    }

    MutableArray<Float64> distributeLinear(size_t points, Float64 max) {

        MutableArray<Float64> arr(points);

        const Float64 frac = max / f64(points);
        
        for(size_t i = 0; i < points; ++i) {
            arr += f64(i) * frac;
        }
        
        return arr;
        
    }
    
    void NyquistPlot::recalculateResponse() {

        if(!filter) return;
        
        path.reset();

        const Point pos = globalTransform().position;
        Point firstPoint = pos;

        const Float64 sr = filter->sampleRate();
        const Float64 nyquist = sr / 2.0;
        constexpr Float64 pi = Math::pi<Float64>;
        constexpr UInt points = 500;
        const Float32 circleRadius = size().x / 4.0f;

        std::complex<Float64> mx = 0.0;
        std::complex<Float64> first = 0.0;
        
        const static auto values = distributeLinear(points, 2.0 * pi);

        for (UInt i = 0; i < points; ++i) {

            const std::complex<Float64> cplx = filter->openTransfer(values[i]);
            const Float64 real = cplx.real();
            const Float64 imag = cplx.imag();
            
            Float64 v = std::sqrt(real * real + imag * imag);

            if(v > std::sqrt(mx.real() * mx.real() + mx.imag() * mx.imag())) {
                mx = cplx;
            }
            
            const Float32 drawX = pos.x + circleRadius * f32(real) + size().x / 2.0f;
            const Float32 drawY = pos.y + circleRadius * f32(imag) + size().y / 2.0f;

            Point newPoint = { drawX, drawY };

            if(i == 0){
                firstPoint = newPoint;
                first = cplx;
                path.moveTo(newPoint.x, newPoint.y);
            }
            else{
                path.lineTo(newPoint.x, newPoint.y);
                
            }
            
        }
        
        path.lineTo(firstPoint.x, firstPoint.y);

    }
    
}
