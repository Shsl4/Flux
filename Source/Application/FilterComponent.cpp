#include <Application/FilterComponent.h>

namespace Flux {

    FilterComponent::FilterComponent(Point const& p) : Component(p, Point(600, 300)){

        constexpr Float32 knobSize = 50.0f;

        const ColorScheme scheme = ColorScheme::coolScheme(Colors::tintGreen);

        stack = Factory::createComponent<HStack>(Point::zero, size(), VAlignment::center, HAlignment::center);
        
        renderer = Factory::createComponent<BodePlot>(Point::zero, Point(500, 300));
        renderer->setScheme(scheme);
        renderer->addListener(this);
        
        frequencyKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        frequencyKnob->setLabelText("Frequency");
        frequencyKnob->setLabelExtension("Hz");
        frequencyKnob->setLabelPrecision(0);
        frequencyKnob->setScheme(scheme);
        frequencyKnob->addListener(this);

        resonanceKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        resonanceKnob->setRange({Audio::Filter::minResonance, Audio::Filter::maxResonance});
        resonanceKnob->setDefaultValue(1.0);
        resonanceKnob->setLabelText("Resonance");
        resonanceKnob->setLabelPrecision(2);
        resonanceKnob->addListener(this);
        resonanceKnob->setScheme(scheme);

        mixKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        mixKnob->setRange({0.0, 1.0});
        mixKnob->setDefaultValue(1.0);
        mixKnob->setLabelText("Mix");
        mixKnob->setLabelPrecision(2);
        mixKnob->addListener(this);
        mixKnob->setScheme(scheme);
        
    }

    void FilterComponent::initialize() {
        
        const auto vStack = Factory::createComponent<VStack>(Point::zero, Point(100, 300), VAlignment::center, HAlignment::center);

        vStack->setVisible(true);
        vStack->setColor(Color::fromHex(0x202020ff));
        vStack->setSpacing(20.0f);
        
        vStack->addChild(frequencyKnob);
        vStack->addChild(resonanceKnob);
        vStack->addChild(mixKnob);

        stack->addChild(renderer);
        stack->addChild(vStack);

        addChild(stack);
        
    }

    void FilterComponent::linkFilter(Audio::Filter* newFilter) {
        this->filter = newFilter;
    }

    void FilterComponent::valueChanged(RotaryKnob* knob, const Float64 newValue) {
        
        if (knob == frequencyKnob) {
            filter->setCutoffFrequency(newValue);
            renderer->recalculatePath();
        }
        
        if (knob == resonanceKnob) {
            filter->setResonance(newValue);
            renderer->recalculatePath();
        }
        
        if (knob == mixKnob) {
            filter->setMix(newValue);
        }
        
    }
    
    void FilterComponent::valueChanged(BodePlot* plot, const Float64 f, const Float64 q) {
                
        filter->setCutoffFrequency(f);
        filter->setResonance(q);

        frequencyKnob->setValue(f, false);
        resonanceKnob->setValue(q, false);
        
    }

    void FilterComponent::setFilter(Audio::LowPassFilter* newFilter) {
        this->filter = newFilter;
        renderer->setFilter(newFilter);

        frequencyKnob->useLogarithmicProgress(true);
        frequencyKnob->setRange({Audio::Filter::minCutoff, (newFilter->sampleRate() / 2.0) * 0.95 });
        frequencyKnob->setDefaultValue(440.0);
        
    }
}
