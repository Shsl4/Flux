#include <Application/FilterComponent.h>
#include <Application/RotaryKnob.h>

namespace Flux {

    FilterComponent::FilterComponent() : Component(Point::zero, { 800, 500 }) {

        this->setColor(Colors::transparent);
        this->filterPlot = Factory::createComponent<BodePlot>(Point::zero, Point(500, 300));
        this->frequencyKnob = Factory::createComponent<RotaryKnob>(Point::zero, 75.0f);
        this->resonanceKnob = Factory::createComponent<RotaryKnob>(Point::zero, 75.0f);

        this->frequencyKnob->setRange({9.0, 22050.0 });
        this->frequencyKnob->setLabelText("Frequency");
        this->frequencyKnob->setLabelExtension(" Hz");
        this->frequencyKnob->setLabelPrecision(0);
        this->frequencyKnob->setDefaultValue(440.0);
        this->frequencyKnob->useLogarithmicProgress(true);

        this->resonanceKnob->setRange({0.1, 6.0 });
        this->resonanceKnob->setLabelText("Resonance");
        this->resonanceKnob->setDefaultValue(1.0);

    }

    void FilterComponent::initialize() {

        auto* mainStack = Factory::createComponent<HStack>(Point::zero, size(), VAlignment::top, HAlignment::left);
        auto* leftStack = Factory::createComponent<VStack>(Point::zero, Point(300, 500), VAlignment::top, HAlignment::center);
        auto* rightStack = Factory::createComponent<VStack>(Point::zero, Point(500, 500), VAlignment::top, HAlignment::center);
        auto* bottomStack = Factory::createComponent<HStack>(Point::zero, Point(500, 200), VAlignment::center, HAlignment::left);

        this->addChild(mainStack);
        mainStack->addChild(leftStack);
        mainStack->addChild(rightStack);
        rightStack->addChild(filterPlot);
        rightStack->addChild(bottomStack);
        bottomStack->addChild(frequencyKnob);
        bottomStack->addChild(resonanceKnob);

        this->frequencyKnob->addListener(this);
        this->resonanceKnob->addListener(this);
        this->filterPlot->addListener(this);

    }

    void FilterComponent::valueChanged(BodePlot *plot, Float64 f, Float64 q) {

        this->frequencyKnob->setValue(f, false);
        this->resonanceKnob->setValue(q, false);

    }

    void FilterComponent::valueChanged(RotaryKnob *knob, Float64 newValue) {

        if(knob == this->frequencyKnob){
            filterPlot->fil()->setCutoffFrequency(newValue);
        }

        if(knob == this->resonanceKnob){
            filterPlot->fil()->setResonance(newValue);
        }

        filterPlot->recalculatePath();

    }

}