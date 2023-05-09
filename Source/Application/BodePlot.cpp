#include <Application/BodePlot.h>
#include <thread>

namespace Flux {
    
    BodePlot::BodePlot(Point const& p, Point const& s): Component(p, s) {

        setColor(scheme.darkest);
        textSize = s.x * 0.0125f;

        this->cplx = MutableArray<kiss_fft_cpx>::filled(spectrumWindowSize);
        this->cplxOut = MutableArray<kiss_fft_cpx>::filled(spectrumWindowSize);
        this->bins = MutableArray<Bin>::filled(spectrumWindowSize);
        this->cfg = kiss_fft_alloc(spectrumWindowSize, 0, nullptr, nullptr);
        this->lastGains = MutableArray<Float64>::filled(spectrumWindowSize);

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

            text->setReactive(false);

            text->setColor(scheme.lightest);
            
            frequencyTexts.add({ value.value(), text });
            
            addChild(text);
            
        }

        for (size_t i = 0; i < 7; ++i) {
            
            Text* text = Factory::createComponent<Text>(Point(0.0f, 0.0f), Point(100, 100), "", textSize, VAlignment::bottom, HAlignment::right);

            text->setReactive(false);

            text->setColor(scheme.lightest);
            
            gainTexts.add(text);
            
            addChild(text);
            
        }

        realignTexts();
        
    }

    void BodePlot::setScheme(const ColorScheme& newScheme) {
        
        this->scheme = newScheme;
        setColor(scheme.darkest);

        for (const auto& text : gainTexts) {
            text->setColor(scheme.lightest);
        }

        for (const auto& entry : frequencyTexts) {
            entry.value()->setColor(scheme.lightest);
        }
        
    }

    void BodePlot::modified() {

        realignTexts();
        recalculatePath();
        
    }

    void BodePlot::realignTexts() const {

        if(!filter) return;
        
        const Range<Float32> logRange = { log10(9.0f), log10(f32(filter->sampleRate()) / 2.0f) };
        const Range<Float32> sizeRange = { size().y, 0.0f };
        const Range<Float32> linRange = Range<Float32>::linear;
        const Point scale = size();

        for (size_t p = 1; p <= 4; ++p) {
            
            const Float32 pw = std::pow(10.0f, f32(p));

            for (size_t d = 1; d <= 9; ++d) {
                
                const Float32 freq = pw * f32(d);
                const Float32 drawX = Range<Float32>::translateValue(log10(freq), logRange, linRange) * scale.x;
                
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
            
            for (const auto& elem : gainsToDraw) {

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

        if(button != MouseButton::Left) return;

        // Setup our range objects
        constexpr auto minCutoff = f32(Audio::Filter::minCutoff);
        const Point pos = globalTransform().position;
        const Range<Float32> logRange = { log10(minCutoff), log10(f32(filter->sampleRate()) / 2.0f) };
        const Range<Float32> freqRange = { minCutoff, (f32(filter->sampleRate()) / 2.0f) * 0.95f };
        const Range<Float32> resonanceRange = { f32(Audio::Filter::minResonance), f32(Audio::Filter::maxResonance) };
        const Range<Float32> hSizeRange = { pos.x, size().x + pos.x };
        const Range<Float32> vSizeRange = { size().y + pos.y, pos.y };

        // Transpose our mouse value to our logarithmic range
        const Float32 logValue = Range<Float32>::translateValue(f32(x), hSizeRange, logRange);

        // Get the frequency value from our log based value (f-1(log10(x)) = 10^x) and clamp it to our frequency range
        const Float32 freq = freqRange.clamp(std::pow(10.0f, logValue));

        // Transpose our mouse value to our resonance range and clamp the value
        const Float32 q = resonanceRange.clamp(Range<Float32>::translateValue(f32(y), vSizeRange, resonanceRange));
        
        filter->setResonance(f64(q));
        filter->setCutoffFrequency(f64(freq));

        for(auto const& listener : listeners) {
            listener->valueChanged(this, f64(freq), f64(q));
        }

        // Recalculate the filter's frequency response.
        recalculatePath();
        
    }

    void BodePlot::draw(Graphics const& graphics) {
        
        graphics.setAntiAliasing(true);
        graphics.setColor(color());
        graphics.drawRect(globalTransform());

        if(filter) {
            drawGrid(graphics);
        }

        recalculateSpectrum();

        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);
        graphics.setStrokeWidth(2.0f);
        graphics.setColor(scheme.base);
        graphics.drawPath(spectrumPath);

        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);
        graphics.setStrokeWidth(2.0f);
        graphics.setColor(scheme.lightest);
        graphics.drawPath(path);

        for (const auto& child : children()) {
            child->draw(graphics);
        }
        
    }

    void BodePlot::setFilter(Audio::Filter* fil) {

        this->filter = fil;
        realignTexts();
        recalculatePath();

        timer.loop(0.01, [this](){
            processFFT();
        });

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

    void BodePlot::drawGrid(Graphics const& graphics) {

        graphics.setAntiAliasing(true);
        graphics.setStrokeWidth(0.5f);
        graphics.setColor(scheme.base);

        const Range<Float32> logRange = { log10(9.0f), log10(f32(filter->sampleRate()) / 2.0f) };
        const Range<Float32> linRange = Range<Float32>::linear;
        const Range<Float32> sizeRange = { size().y, 0.0f };
        const Point position = globalTransform().position;
        const Point scale = size();

        if(mode == DrawMode::frequency) {
            
            for (const auto& elem : gainsToDraw) {

                const auto drawY = Range<Float32>::translateValue(elem, gainRange, sizeRange);
                graphics.drawLine({position.x, drawY + position.y}, {position.x + scale.x, drawY + position.y});

            }
            
        }
        else {

            for (const auto& elem : phasesToDraw) {

                const auto drawY = Range<Float32>::translateValue(elem, phaseRange, sizeRange);
                graphics.drawLine({position.x, drawY + position.y}, {position.x + scale.x, drawY + position.y});

            }
            
        }

        for (size_t p = 1; p <= 4; ++p) {

            const Float32 pw = std::pow(10.0f, f32(p));

            for (size_t d = 1; d <= 9; ++d) {

                const Float32 freq = pw * f32(d);
                const Float32 drawX = Range<Float32>::translateValue(log10(freq), logRange, linRange) * scale.x + position.x;

                if(drawX > position.x + scale.x) return;

                graphics.drawLine({drawX, position.y}, {drawX, position.y + scale.y});

            }

        }
        
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
        constexpr UInt points = 300;
        constexpr Float64 mindB = -20.0;
        constexpr Float64 maxdB = 20.0;

        const auto values = distributeAround(f32(filter->cutoff()), nyquist, points);

        for (UInt i = 0; i < points; ++i) {

            const Float64 mag = filter->magnitude(values[i]);
            const Float64 response = 20.0 * log10(mag);
            const Float64 freq = (values[i] / pi) * nyquist;
            const Float64 finalResponse = Math::clamp(response, mindB, maxdB);
            const Range<Float64> logRange = { log10(9.0), log10(nyquist) };
            const Range<Float64>& linRange = Range<Float64>::linear;

            const Float64 normalizedResponse = (finalResponse - mindB) / (maxdB - mindB);
            const Float64 normalizedFrequency = Range<Float64>::translateValue(log10(freq), logRange, linRange);

            if(!std::isfinite(normalizedResponse)) continue;

            const Float32 drawX = pos.x + f32(normalizedFrequency) * size().x;
            const Float32 drawY = pos.y + size().y - f32(normalizedResponse) * size().y;

            Point newPoint = { drawX, drawY };

            if(i == 0){
                path.moveTo(newPoint.x, newPoint.y);
            }
            else{

                path.moveTo(lastPoint.x, lastPoint.y);

                if(Math::dneq(finalResponse, mindB)){
                    path.lineTo(newPoint.x, newPoint.y);
                }
                else if(lastPoint.y != (pos.y + size().y)){
                    path.lineTo(newPoint.x, pos.y + size().y);
                }

            }

            lastPoint = newPoint;

        }
        
    }

    void BodePlot::recalculateSpectrum() {

        spectrumPath.reset();

        const Point pos = globalTransform().position;
        Point lastPoint = pos;
        lastPoint.y += size().y;

        const Float64 sr = filter->sampleRate();
        const Float64 nyquist = sr / 2.0;
        const size_t points = bins.size();
        constexpr Float64 mindB = -20.0;
        constexpr Float64 maxdB = 20.0;

        for (UInt i = 0; i < points; i++) {

            const size_t prev = i == 0 ? 0 : i - 1;
            const size_t next = i + 1 == points ? i : i + 1;

            const Float64 previousResponse = (bins[prev].gain + lastGains[prev]) / 2.0;
            const Float64 currentResponse = (bins[i].gain + lastGains[i]) / 2.0;
            const Float64 nextResponse = (bins[next].gain + lastGains[next]) / 2.0;
            const Float64 response = (previousResponse + currentResponse + nextResponse) / 3.0;
            const Float64 freq = Math::clamp(bins[i].frequency, 9.0, nyquist);
            const Float64 finalResponse = Math::clamp(response, mindB, maxdB);
            const Range<Float64> logRange = { log10(9.0), log10(nyquist) };
            const Range<Float64>& linRange = Range<Float64>::linear;

            const Float64 normalizedResponse = (finalResponse - mindB) / (maxdB - mindB);
            const Float64 normalizedFrequency = Range<Float64>::translateValue(log10(freq), logRange, linRange);

            if(!std::isfinite(response)) continue;

            const Float32 drawX = pos.x + f32(normalizedFrequency) * size().x;
            const Float32 drawY = pos.y + size().y - f32(normalizedResponse) * size().y;

            Point newPoint = { drawX, drawY };

            if(i == 0){
                spectrumPath.moveTo(newPoint.x, newPoint.y);
            }
            else{
                spectrumPath.lineTo(newPoint.x, newPoint.y);
            }

            lastGains[i] = response;

        }

    }

    void BodePlot::recalculatePhaseResponse() {

        if(!filter) return;
        
        path.reset();

        const Point pos = globalTransform().position;
        Point lastPoint = pos;
        lastPoint.y += size().y;

        constexpr Float64 pi = Math::pi<Float64>;
        constexpr UInt points = 300;

        const Float64 sr = filter->sampleRate();
        const Float64 nyquist = sr / 2.0;

        const auto values = distributeAround(f32(filter->cutoff()), nyquist, points);

        for (UInt i = 0; i < points; ++i) {

            const Float64 phase = filter->argument(values[i]) - Math::pi<Float64>;
            const Float64 response = fmod(Math::toDegrees(phase), 360);
            const Float64 freq = (values[i] / pi) * nyquist;
            const Range logRange = { log10(9.0), log10(nyquist) };
            const Range<Float64>& linRange = Range<Float64>::linear;

            const Float64 normalizedResponse = Range<Float64>::translateValue(response, phaseRange64, linRange);
            const Float64 normalizedFrequency = Range<Float64>::translateValue(log10(freq), logRange, linRange);

            if(!std::isfinite(normalizedResponse)) continue;

            const Float32 drawX = pos.x + f32(normalizedFrequency) * size().x;
            const Float32 drawY = pos.y + size().y - f32(normalizedResponse) * size().y;

            const Point newPoint = { drawX, drawY };

            if(i == 0){
                path.moveTo(newPoint.x, newPoint.y);
            }
            else{

                path.moveTo(lastPoint.x, lastPoint.y);

                if(Math::dneq(response, phaseRange64.min())){
                    path.lineTo(newPoint.x, newPoint.y);
                }
                else if(lastPoint.y != (pos.y + size().y)){
                    path.lineTo(newPoint.x, pos.y + size().y);
                }

            }

            lastPoint = newPoint;

        }
        
    }

    void BodePlot::addListener(BodePlot::Listener *listener) {
        if(listener) listeners += listener;
    }

    void BodePlot::removeListener(BodePlot::Listener *listener) {
        listeners -= listener;
    }

    void BodePlot::feedBuffer(Float64 *block) {
        circularBuffer.feed(block, filter->bufferSize());
    }

    void BodePlot::willDispose() {
        kiss_fft_free(cfg);
    }

    void BodePlot::processFFT() {

        constexpr auto size = spectrumWindowSize;
        constexpr auto fSize = f64(size);

        const Float64 sampleRate = filter->sampleRate();
        const Float64* values = circularBuffer.buffer.data();

        for (size_t i = 0; i < size; ++i) {
            cplx[i].r = values[i] * 0.5 * (1.0 - std::cos((2.0 * Math::pi<Float64> * f64(i)) / (fSize - 1.0)));
            cplx[i].i = 0.0;
        }

        kiss_fft(cfg, cplx.data(), cplxOut.data());

        for (size_t i = 0; i < size; ++i) {

            const auto magnitude = std::sqrt(cplxOut[i].r * cplxOut[i].r + cplxOut[i].i * cplxOut[i].i);
            bins[i].gain = Flux::Audio::toDecibels(magnitude);
            bins[i].frequency = f64(i) * sampleRate / fSize;

        }

    }

}
