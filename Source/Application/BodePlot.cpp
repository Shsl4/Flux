#include <Application/BodePlot.h>
#include <thread>

namespace Flux {
    
    BodePlot::BodePlot(Point const& p, Point const& s): Component(p, s) {

        setColor(scheme.darkest);
        textSize = s.x * 0.0125f;

        this->fftIn = MutableArray<std::complex<Float64>>::filled(spectrumWindowSize);
        this->fftOut = MutableArray<std::complex<Float64>>::filled(spectrumWindowSize);
        this->bins = MutableArray<Bin>::filled(spectrumWindowSize);
        this->lastGains = MutableArray<Float64>::filled(spectrumWindowSize);
        
        memset(bins.data(), 0, bins.size() * sizeof(Bin));
        memset(lastGains.data(), 0, lastGains.size() * sizeof(Float64));

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

        for (size_t i = 0; i < gainsToDraw.size(); ++i) {
            
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
        
        const Range<Float32> sizeRange = { size().y, 0.0f };
        const Point scale = size();

        for (size_t p = 1; p <= 4; ++p) {
            
            const Float32 pw = std::pow(10.0f, f32(p));

            for (size_t d = 1; d <= 9; ++d) {
                
                const Float32 freq = pw * f32(d);
                const Float32 drawX = logFrequencyRange<Float32>().translateTo(log10(freq), Range<Float32>::linear) * scale.x;
                
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
                const Float32 drawY = gainRange<Float32>.translateTo(elem, sizeRange);
                text->setText(String::format("{}dB", i32(elem)));
                text->setPosition({ size().x - text->size().x, drawY - text->size().y });
                ++i;

            }
            
        }
        else {

            size_t i = 0;
            
            for (auto& elem : phasesToDraw) {

                Text* text = gainTexts[i];
                const Float32 drawY = phaseRange<Float32>.translateTo(elem, sizeRange);
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
        const Point pos = globalTransform().position;
        const Range<Float32> hSizeRange = { pos.x, size().x + pos.x };
        const Range<Float32> vSizeRange = { size().y + pos.y, pos.y };

        // Transpose our mouse value to our logarithmic range
        const Float32 logValue = hSizeRange.translateTo(f32(x), logFrequencyRange<Float32>());

        // Get the frequency value from our log based value (f-1(log10(x)) = 10^x) and clamp it to our frequency range
        const Float32 freq = frequencyRange<Float32>().clamp(std::pow(10.0f, logValue));

        // Transpose our mouse value to our resonance range and clamp the value
        const Float32 q = resonanceRange<Float32>.clamp(vSizeRange.translateTo(f32(y), resonanceRange<Float32>));
        
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

        std::lock_guard m(mutex);
        
        graphics.setStrokeStyle(Graphics::StrokeStyle::fill);
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

        timer.stop();
        
        // Refresh the spectrum every 10ms
        timer.loop(0.01, [this] {
            std::lock_guard m(mutex);
            processFFT();
            recalculateSpectrum();
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

        const Range<Float32> linRange = Range<Float32>::linear;
        const Range<Float32> sizeRange = { size().y, 0.0f };
        const Point position = globalTransform().position;
        const Point scale = size();

        if(mode == DrawMode::frequency) {

            const auto range = gainRange<Float32>;
            
            for (const auto& elem : gainsToDraw) {

                const auto drawY = range.translateTo(elem, sizeRange);
                graphics.drawLine({position.x, drawY + position.y}, {position.x + scale.x, drawY + position.y});

            }
            
        }
        else {

            const auto range = phaseRange<Float32>;

            for (const auto& elem : phasesToDraw) {

                const auto drawY = range.translateTo(elem, sizeRange);
                graphics.drawLine({position.x, drawY + position.y}, {position.x + scale.x, drawY + position.y});

            }
            
        }

        const Range<Float32> fRange = logFrequencyRange<Float32>();
        
        for (size_t p = 1; p <= 4; ++p) {

            const Float32 pw = std::pow(10.0f, f32(p));

            for (size_t d = 1; d <= 9; ++d) {

                const Float32 freq = pw * f32(d);
                const Float32 drawX = fRange.translateTo(log10(freq), linRange) * scale.x + position.x;

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

        constexpr Float64 pi = Math::pi<Float64>;
        const Float64 ny = nyquist();

        const auto values = distributeAround(f32(filter->cutoff()), ny, filterResponsePoints);

        const Range<Float64> logRange = logFrequencyRange<Float64>();
        const Range<Float64>& linRange = Range<Float64>::linear;
        
        for (UInt i = 0; i < filterResponsePoints; ++i) {

            const Float64 mag = filter->magnitude(values[i]);
            const Float64 response = 20.0 * log10(mag);
            const Float64 freq = (values[i] / pi) * ny;
            const Float64 finalResponse = Math::clamp(response, mindB, maxdB);

            const Float64 normalizedResponse = (finalResponse - mindB) / (maxdB - mindB);
            const Float64 normalizedFrequency = logRange.translateTo(log10(freq), linRange);

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
                else if(Math::fneq(lastPoint.y, pos.y + size().y)){
                    path.lineTo(newPoint.x, pos.y + size().y);
                }

            }

            lastPoint = newPoint;

        }
        
    }

    void BodePlot::recalculateSpectrum() {

        spectrumPath.reset();

        const Transform t = globalTransform();
        const Point pos = t.position;
        constexpr Float64 ratio = 0.1;

        const size_t points = spectrumWindowSize;
        const Range<Float64> logRange = logFrequencyRange<Float64>();
        const Range<Float64> fRange = frequencyRange<Float64>();
        const Range<Float64>& linRange = Range<Float64>::linear;
        
        spectrumPath.moveTo(pos.x, pos.y + t.size.y);

        for (UInt i = 0; i < points; i++) {

            const size_t prev = i == 0 ? 0 : i - 1;
            const size_t next = i + 1 == points ? i : i + 1;
            
            const Float64 previousResponse = Math::lerp(lastGains[prev], bins[prev].gain, ratio);
            const Float64 currentResponse = Math::lerp(lastGains[i], bins[i].gain, ratio);
            const Float64 nextResponse = Math::lerp(lastGains[next], bins[next].gain, ratio);
            const Float64 response = (previousResponse + currentResponse + nextResponse) / 3.0;
            const Float64 freq = fRange.clamp(bins[i].frequency);
            const Float64 finalResponse = gainRange<Float64>.clamp(response);

            const Float64 normalizedResponse = (finalResponse - mindB) / (maxdB - mindB);
            const Float64 normalizedFrequency = logRange.translateTo(log10(freq), linRange);

            if(!std::isfinite(response) || std::isnan(response)) continue;

            const Float32 drawX = pos.x + f32(normalizedFrequency) * size().x;
            const Float32 drawY = pos.y + size().y - f32(normalizedResponse) * size().y;

            Point newPoint = { drawX, drawY };

            spectrumPath.lineTo(newPoint.x, newPoint.y);

            lastGains[i] = finalResponse;

        }

        spectrumPath.lineTo(pos.x + t.size.x, pos.y + t.size.y);

    }

    void BodePlot::recalculatePhaseResponse() {

        if(!filter) return;
        
        path.reset();

        const Point pos = globalTransform().position;
        Point lastPoint = pos;
        lastPoint.y += size().y;

        constexpr Float64 pi = Math::pi<Float64>;
        const Float64 ny = nyquist();

        const auto values = distributeAround(f32(filter->cutoff()), ny, filterResponsePoints);
        
        const Range logRange = logFrequencyRange<Float64>();
        const Range<Float64>& linRange = Range<Float64>::linear;

        for (UInt i = 0; i < filterResponsePoints; ++i) {

            const Float64 phase = filter->argument(values[i]) - Math::pi<Float64>;
            const Float64 response = std::fmod(Math::toDegrees(phase), 360);
            const Float64 freq = values[i] / pi * ny;

            const Float64 normalizedResponse = phaseRange<Float64>.translateTo(response, linRange);
            const Float64 normalizedFrequency = logRange.translateTo(log10(freq), linRange);

            if(!std::isfinite(normalizedResponse)) continue;

            const Float32 drawX = pos.x + f32(normalizedFrequency) * size().x;
            const Float32 drawY = pos.y + size().y - f32(normalizedResponse) * size().y;

            const Point newPoint = { drawX, drawY };

            if(i == 0){
                path.moveTo(newPoint.x, newPoint.y);
            }
            else{

                path.moveTo(lastPoint.x, lastPoint.y);

                if(Math::dneq(response, phaseRange<Float64>.min())){
                    path.lineTo(newPoint.x, newPoint.y);
                }
                else if(Math::fneq(lastPoint.y, pos.y + size().y)){
                    path.lineTo(newPoint.x, pos.y + size().y);
                }

            }

            lastPoint = newPoint;

        }
        
    }

    void BodePlot::addListener(Listener*listener) {
        if(listener) listeners += listener;
    }

    void BodePlot::removeListener(Listener*listener) {
        listeners -= listener;
    }

    void BodePlot::feedBuffer(const Float64 *block) {
        circularBuffer.feed(block, filter->bufferSize());
    }

    void BodePlot::willDispose() {

        timer.stop();
        
    }

    void BodePlot::processFFT() const {
        
        constexpr auto size = spectrumWindowSize;
        constexpr auto fSize = f64(size);

        const Float64 sampleRate = filter->sampleRate();
        const Float64* values = circularBuffer.buffer.data();

        for (size_t i = 0; i < size; ++i) {
            auto windowed = values[i] * 0.5 * (1.0 - std::cos(2.0 * Math::pi<Float64> * f64(i) / (fSize - 1)));
            fftIn[i] = { windowed, 0.0 };
        }

        kissFFT.transform(fftIn.data(), fftOut.data());
        
        for (size_t i = 0; i < size; ++i) {
            
            bins[i].gain = Audio::toDecibels(std::abs(fftOut[i]) / std::sqrt(fSize));
            bins[i].frequency = f64(i) * sampleRate / fSize;

        }

    }

}
