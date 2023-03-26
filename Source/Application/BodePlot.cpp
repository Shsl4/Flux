#include <Application/BodePlot.h>

namespace Flux {
    
    BodePlot::BodePlot(Point const& p, Point const& s): Component(p, s) {

        setColor(scheme.darkest);
        textSize = s.x * 0.0125f;
        
    }

    void BodePlot::initialize() {
        
        const Map<String, Float32> frequencyValues = {
            
            { "10", 10.0f },
            { "20", 20.0f },
            { "50", 50.0f },
            { "100", 100.0f },
            { "200", 200.0f },
            { "500", 500.0f },
            { "1k", 1000.0f },
            { "2k", 2000.0f },
            { "5k", 5000.0f },
            { "10k", 10000.0f },
            { "20k", 20000.0f }
            
        };
        
        for(auto const& value : frequencyValues) {

            Text* text = Factory::createComponent<Text>(Point(0.0f, 0.0f), Point(100, 100),
                value.key(), textSize, VAlignment::bottom, HAlignment::center);

            text->setColor(scheme.lightest);
            
            frequencyTexts.add({ value.value(), text });
            
            addChild(text);
            
        }

        for (size_t i = 0; i < 7; ++i) {
            
            Text* text = Factory::createComponent<Text>(Point(0.0f, 0.0f), Point(100, 100), "", textSize, VAlignment::bottom, HAlignment::right);

            text->setColor(scheme.lightest);
            
            gainTexts.add(text);
            
            addChild(text);
            
        }

        realignTexts();
        
    }
    
    void BodePlot::modified() {

        realignTexts();
        
    }

    void BodePlot::realignTexts() const {

        if(!filter) return;
        
        const Range logRange = { log10(9.0f), log10(f32(filter->sampleRate()) / 2.0f) };
        const Range sizeRange = { size().y, 0.0f };
        const Range<Float32> linRange = Range<Float32>::makeLinearRange();
        const Point position = globalTransform().position;
        const Point scale = size();

        for (size_t p = 1; p <= 4; ++p) {
            
            const Float32 pw = std::pow(10.0f, f32(p));

            for (size_t d = 1; d <= 9; ++d) {
                
                const Float32 freq = pw * f32(d);
                const Float32 drawX = Range<Float32>::translateValue(log10(freq), logRange, linRange)* scale.x + position.x;
                
                if(frequencyTexts.containsKey(freq)) {

                    Text* text = frequencyTexts[freq];
                    text->setPosition({ drawX - text->size().x / 2.0f, size().y - text->size().y });
                    
                }
                
            }

        }

        for(auto const& text : gainTexts) {
            text->setText("");
        }

        if(mode == DrawMode::frequency) {

            size_t i = 0;
            
            for (auto& elem : gainsToDraw) {

                Text* text = gainTexts[i];
                const Float32 drawY = Range<Float32>::translateValue(elem, gainRange, sizeRange);
                text->setText(String::format("{}dB", i32(elem)));
                text->setPosition({ size().x - text->size().x, drawY - text->size().y });
                ++i;

            }
            
        }
        else {

            size_t i = 0;
            
            for (auto& elem : phasesToDraw) {

                Text* text = gainTexts[i];
                const Float32 drawY = Range<Float32>::translateValue(elem, phaseRange, sizeRange);
                text->setText(String::format("{}", i32(elem)));
                text->setPosition({ size().x - text->size().x, drawY - text->size().y });
                ++i;

            }
            
        }

    }

    void BodePlot::doubleClick(MouseButton button, Float64 x, Float64 y) {
        Component::doubleClick(button, x, y);
    }

    void BodePlot::drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        // Setup our range objects
        const Range logRange = { log10(9.0f), log10(f32(filter->sampleRate()) / 2.0f) };
        const Range freqRange = { 9.0f, (f32(filter->sampleRate()) / 2.0f) * 0.95f };
        const Range resonanceRange = { 0.1f, 6.0f };
        const Range hSizeRange = { 0.0f, size().x };
        const Range vSizeRange = { size().y, 0.0f };

        // Transpose our mouse value to our logarithmic range
        const Float32 logValue = Range<Float32>::translateValue(f32(x), hSizeRange, logRange);

        // Get the frequency value from our log based value (f-1(log10(x)) = 10^x) and clamp it to our frequency range
        const Float32 freq = freqRange.clamp(std::pow(10.0f, logValue));

        // Transpose our mouse value to our resonance range and clamp the value
        const Float32 q = resonanceRange.clamp(Range<Float32>::translateValue(f32(y), vSizeRange, resonanceRange));
        
        filter->setResonance(f64(q));
        filter->setCutoffFrequency(f64(freq));

        // Recalculate the filter's frequency response.
        recalculatePath();
        
    }

    void BodePlot::draw(SkCanvas* canvas, Float64 deltaTime) {
        
        SkPaint paint;
        const auto t = globalTransform();
        const SkRect rect = SkRect::MakeXYWH(t.position.x, t.position.y, t.size.x, t.size.y);

        paint.setColor(color().skColor());
        canvas->drawRect(rect, paint);
        
        if(filter) {
            drawGrid(canvas, deltaTime);
        }

        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(2.0f);
        paint.setColor(scheme.lightest.skColor());
        
        canvas->drawPath(path, paint);

        for (const auto& child : children()) {
            child->draw(canvas, deltaTime);
        }
        
    }

    void BodePlot::setFilter(Audio::Filter* fil) {
        this->filter = fil;
        realignTexts();
        recalculatePath();
    }

    void BodePlot::keyDown(Key const& key) {

        if(key.key == 32) {

            if(mode == DrawMode::frequency) {
                this->mode = DrawMode::phase;
            }
            else {
                this->mode = DrawMode::frequency;
            }

            realignTexts();
            recalculatePath();
            
        }
        
    }

    void BodePlot::drawGrid(SkCanvas* canvas, Float64 deltaTime) {

        SkPaint paint;
        paint.setStrokeWidth(0.5f);
        paint.setColor(scheme.base.skColor());

        const Range logRange = { log10(9.0f), log10(f32(filter->sampleRate()) / 2.0f) };
        const Range<Float32> linRange = Range<Float32>::makeLinearRange();
        const Range sizeRange = { size().y, 0.0f };
        const Point position = globalTransform().position;
        const Point scale = size();

        if(mode == DrawMode::frequency) {
            
            for (auto& elem : gainsToDraw) {

                const Float32 drawY = Range<Float32>::translateValue(elem, gainRange, sizeRange);
                canvas->drawLine(position.x, drawY, position.x + scale.x, drawY, paint);

            }
            
        }
        else {

            for (auto& elem : phasesToDraw) {

                const Float32 drawY = Range<Float32>::translateValue(elem, phaseRange, sizeRange);
                canvas->drawLine(position.x, drawY, position.x + scale.x, drawY, paint);

            }
            
        }
        
        for (size_t p = 1; p <= 4; ++p) {
            
            const Float32 pw = std::pow(10.0f, f32(p));

            for (size_t d = 1; d <= 9; ++d) {
                
                const Float32 freq = pw * f32(d);
                const Float32 drawX = Range<Float32>::translateValue(log10(freq), logRange, linRange) * scale.x + position.x;

                if(drawX > position.x + scale.x) return;

                canvas->drawLine(drawX, position.y, drawX, position.y + scale.y, paint);

                if(frequencyTexts.containsKey(freq)) {

                    Text* text = frequencyTexts[freq];
                    text->setPosition({ drawX - text->size().x / 2.0f, size().y - text->size().y });
                    
                }
                
            }

        }
        
    }
    
    Float64 freqToRad(Float64 f, Float64 ny) {
        return (f * Math::pi<Float64>) / ny;
    }
    
    MutableArray<Float64> distributeAround(Float64 freq, Float64 ny, UInt count) {

        auto values = MutableArray<Float64>(count);
        const Float64 baseRad = freqToRad(9.0, ny);

        // todo: distribute around f
        for (size_t i = 0; i < count; ++i) {
            values += baseRad + (Math::pi<Float64> - baseRad) * pow(f64(i) / f64(count), 5.0);
        }

        return values;

    }

    void BodePlot::recalculatePath() {

        if(mode == DrawMode::frequency) {
            recalculateFrequencyResponse();
        }
        else {
            recalculatePhaseResponse();
        }
        
    }
    
    void BodePlot::recalculateFrequencyResponse() {

        if(!filter) return;
        
        path.reset();

        const Point pos = globalTransform().position;
        Point lastPoint = pos;
        lastPoint.y += size().y;

        const Float64 sr = filter->sampleRate();
        const Float64 nyquist = sr / 2.0;
        constexpr Float64 pi = Math::pi<Float64>;
        constexpr UInt points = 500;
        constexpr Float64 mindB = -20.0;
        constexpr Float64 maxdB = 20.0;

        const auto values = distributeAround(f32(filter->cutoff()), nyquist, points);

        for (UInt i = 0; i < points; ++i) {

            const Float64 mag = filter->magnitude(values[i]);
            const Float64 response = 20.0 * log10(mag);
            const Float64 freq = (values[i] / pi) * nyquist;
            const Float64 finalResponse = Math::clamp(response, mindB, maxdB);
            const Range<Float64> logRange = { log10(9.0), log10(nyquist) };
            const Range<Float64> linRange = Range<Float64>::makeLinearRange();

            const Float64 normalizedResponse = (finalResponse - mindB) / (maxdB - mindB);
            const Float64 normalizedFrequency = Range<Float64>::translateValue(log10(freq), logRange, linRange);

            if(!isfinite(normalizedResponse)) continue;

            const Float32 drawX = pos.x + f32(normalizedFrequency) * size().x;
            const Float32 drawY = pos.y + size().y - f32(normalizedResponse) * size().y;

            assert(drawX < size().x);
            
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

    void BodePlot::recalculatePhaseResponse() {

        if(!filter) return;
        
        path.reset();

        const Point pos = globalTransform().position;
        Point lastPoint = pos;
        lastPoint.y += size().y;

        constexpr Float64 pi = Math::pi<Float64>;
        constexpr UInt points = 500;

        const Float64 sr = filter->sampleRate();
        const Float64 nyquist = sr / 2.0;

        const auto values = distributeAround(f32(filter->cutoff()), nyquist, points);

        for (UInt i = 0; i < points; ++i) {

            const Float64 phase = filter->argument(values[i]) - Math::pi<Float64>;
            const Float64 response = fmod(Math::toDegrees(phase), 360);
            const Float64 freq = (values[i] / pi) * nyquist;
            const Range logRange = { log10(9.0), log10(nyquist) };
            const Range<Float64> linRange = Range<Float64>::makeLinearRange();

            const Float64 normalizedResponse = Range<Float64>::translateValue(response, phaseRange64, linRange);
            const Float64 normalizedFrequency = Range<Float64>::translateValue(log10(freq), logRange, linRange);

            if(!isfinite(normalizedResponse)) continue;

            const Float32 drawX = pos.x + f32(normalizedFrequency) * size().x;
            const Float32 drawY = pos.y + size().y - f32(normalizedResponse) * size().y;

            const Point newPoint = { drawX, drawY };

            if(i == 0){
                path.moveTo(newPoint.x, newPoint.y);
            }
            else{

                path.moveTo(lastPoint.x, lastPoint.y);

                if(Math::dneq(response, phaseRange64.min()))
                    path.lineTo(newPoint.x, newPoint.y);
                
            }

            lastPoint = newPoint;

        }
        
    }
    
}
