#include <Application/NyquistPlot.h>

namespace Flux {
    
    NyquistPlot::NyquistPlot(Point const& p, Point const& s): Component(p, s) {
        setColor(scheme.darkest);
        textSize = s.x * 0.025f;
    }
    
    void NyquistPlot::draw(Graphics const& graphics) {

        const Transform transform = globalTransform();

        graphics.setColor(color());
        graphics.drawRect(transform);
        
        drawCircle(graphics);

        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);
        graphics.setStrokeWidth(2.0f);
        graphics.setColor(scheme.lightest);

        graphics.save();
        graphics.clip(transform.position, transform.size);
        
        graphics.drawPath(path);

        for (const auto& child : children()) {
            child->draw(graphics);
        }

        graphics.restore();
        
    }
    
    void NyquistPlot::drawCircle(Graphics const& graphics) const {

        graphics.setStrokeWidth(2.0f);
        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);
        graphics.setColor(scheme.base);

        const Point position = globalTransform().position;
        const Point scale = size();
        
        graphics.drawCircle({position.x + scale.x / 2.0f, position.y + scale.y / 2.0f}, scale.x / 4.0f);

        graphics.setStrokeWidth(1.0f);

        graphics.drawLine({position.x + scale.x / 2.0f, position.y}, {position.x + scale.x / 2.0f, position.y + scale.y});
        graphics.drawLine({position.x, position.y + scale.y / 2.0f}, {position.x + scale.x, position.y + scale.y / 2.0f});
        
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
        
        const static auto values = distributeLinear(points, 2.0 * pi);

        for (UInt i = 0; i < points; ++i) {

            const std::complex<Float64> cplx = filter->openTransfer(values[i]);
            const Float64 real = cplx.real();
            const Float64 imag = cplx.imag();

            const Float32 drawX = pos.x + circleRadius * f32(real) + size().x / 2.0f;
            const Float32 drawY = pos.y + circleRadius * f32(imag) + size().y / 2.0f;

            Point newPoint = { drawX, drawY };

            if(i == 0){
                firstPoint = newPoint;
                path.moveTo(newPoint.x, newPoint.y);
            }
            else{
                path.lineTo(newPoint.x, newPoint.y);
                
            }
            
        }
        
        path.lineTo(firstPoint.x, firstPoint.y);

    }
    
}
