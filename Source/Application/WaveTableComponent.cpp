#include <Application/WaveTableComponent.h>

namespace Flux {

    WaveTableComponent::WaveTableComponent(const Point &pos) : Component(pos, Point(300, 300)){

        constexpr Float32 knobSize = 50;

        stack = Factory::createComponent<VStack>(Point::zero, size(), VAlignment::center, HAlignment::center);

        frameKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        frameKnob->setRange({0, 10});
        frameKnob->setDefaultValue(0);
        frameKnob->setLabelText("Frame");
        frameKnob->setLabelPrecision(0);
        frameKnob->addListener(this);

        phaseKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        phaseKnob->setRange({0, 360});
        phaseKnob->setDefaultValue(180);
        phaseKnob->setLabelText("Phase");
        phaseKnob->setLabelPrecision(2);
        phaseKnob->addListener(this);

        panKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        panKnob->setRange({-1, 1});
        panKnob->setDefaultValue(0);
        panKnob->setLabelText("Pan");
        panKnob->setLabelPrecision(2);
        panKnob->addListener(this);

        levelKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        levelKnob->setRange({0.0, 1.0});
        levelKnob->setDefaultValue(0.8);
        levelKnob->setLabelText("Level");
        levelKnob->setLabelPrecision(2);
        levelKnob->addListener(this);

        attackKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        attackKnob->setRange({0.0, 5.0});
        attackKnob->setDefaultValue(0.005);
        attackKnob->setLabelText("Attack");
        attackKnob->setLabelPrecision(3);
        attackKnob->addListener(this);

        decayKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        decayKnob->setRange({0.0, 5.0});
        decayKnob->setDefaultValue(0.5);
        decayKnob->setLabelText("Decay");
        decayKnob->setLabelPrecision(2);
        decayKnob->addListener(this);

        sustainKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        sustainKnob->setRange({0.0, 1.0});
        sustainKnob->setDefaultValue(1.0);
        sustainKnob->setLabelText("Sustain");
        sustainKnob->setLabelPrecision(2);
        sustainKnob->addListener(this);

        releaseKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        releaseKnob->setRange({0.0, 5.0});
        releaseKnob->setDefaultValue(0.1);
        releaseKnob->setLabelText("Release");
        releaseKnob->setLabelPrecision(2);
        releaseKnob->addListener(this);

        renderer = Factory::createComponent<WaveTableRenderer>(Point::zero, Point(300, 150));

    }

    void WaveTableComponent::initialize() {
        
        const auto hStack = Factory::createComponent<HStack>(Point::zero, Point(size().x, 75), VAlignment::center, HAlignment::center);
        const auto hStack2 = Factory::createComponent<HStack>(Point::zero, Point(size().x, 75), VAlignment::center, HAlignment::center);

        hStack->setVisible(true);
        hStack->setColor(Color::fromHex(0x252525ff));
        hStack->setSpacing(20.0f);
        
        hStack2->setVisible(true);
        hStack2->setColor(Color::fromHex(0x252525ff));
        hStack2->setSpacing(20.0f);

        hStack->addChild(frameKnob);
        hStack->addChild(phaseKnob);
        hStack->addChild(panKnob);
        hStack->addChild(levelKnob);

        hStack2->addChild(attackKnob);
        hStack2->addChild(decayKnob);
        hStack2->addChild(sustainKnob);
        hStack2->addChild(releaseKnob);

        addChild(stack);
        
        stack->addChild(renderer);
        stack->addChild(hStack);
        stack->addChild(hStack2);
        
    }

    void WaveTableComponent::valueChanged(Slider *knob, Float64 newValue) {

        if(knob == frameKnob){

            const auto newFrame = st(std::floor(newValue));

            if(newFrame != waveTable->selectedFrame()){
                waveTable->setCurrentFrame(newFrame);
                renderer->calculateWaveform(waveTable->frame(newFrame));
            }

        }

        if(knob == levelKnob){
            waveTable->setLevel(newValue);
        }

        if (knob == attackKnob) {
            waveTable->setAttack(newValue);
        }

        if (knob == decayKnob) {
            waveTable->setDecay(newValue);
        }
        
        if (knob == sustainKnob) {
            waveTable->setSustain(newValue);
        }

        if (knob == releaseKnob) {
            waveTable->setRelease(newValue);
        }

    }

    void WaveTableComponent::linkWaveTable(WaveTable *wt) {

        this->waveTable = wt;

        frameKnob->setRange({0, f64(waveTable->maxFrames() - 1)});
        frameKnob->setValue(0);

        levelKnob->setValue(waveTable->currentLevel());

        renderer->calculateWaveform(waveTable->frame(0));

    }

    void WaveTableComponent::WaveTableRenderer::draw(const Graphics &graphics) {

        graphics.setColor(color());
        graphics.drawRect(globalTransform());

        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);
        graphics.setStrokeWidth(2.0f);
        graphics.setAntiAliasing(true);
        graphics.setColor(scheme.lightest);
        graphics.drawPath(path);

    }

    void WaveTableComponent::WaveTableRenderer::initialize() {
        Component::initialize();
    }

    WaveTableComponent::WaveTableRenderer::WaveTableRenderer(const Point &p, const Point &s) : Component(p, s) {
        setColor(scheme.darkest);
    }

    void WaveTableComponent::WaveTableRenderer::calculateWaveform(const MutableArray<Float64>& data) {

        path.reset();

        const Point pos = globalTransform().position;

        for(size_t i = 0; i < data.size(); ++i) {

            const Float64 value = (Math::clamp(data[i], -1.0, 1.0) + 1.0) / 2.0;
            const Float32 drawX = pos.x + size().x * (f32(i) / f32(data.size()));
            const Float32 drawY = pos.y + (size().y * f32(value)) * 0.9f + 0.05f * size().y;

            const Point newPoint = { drawX, drawY };

            if(i == 0) {
                path.moveTo(newPoint.x, newPoint.y);
            }
            else{
                path.lineTo(newPoint.x, newPoint.y);
            }

        }

    }

    void WaveTableComponent::WaveTableRenderer::modified() {

        Component::modified();

        const auto* wtComp = dynamic_cast<WaveTableComponent*>(parent()->parent());

        if(wtComp && wtComp->waveTable){
            calculateWaveform(wtComp->waveTable->frame(wtComp->waveTable->selectedFrame()));
        }

    }

}


