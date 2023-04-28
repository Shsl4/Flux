#include <Application/WaveTableModule.h>

namespace Flux {

    void WaveTableModule::initialize() {

        this->waveTable = Shared<WaveTable>::make(Factory::loadWaveFile(FLUX_RESOURCES "/Audio/Basic Shapes.wav"));
        this->wtComponent = Factory::createComponent<WaveTableComponent>();
        this->wtComponent->setWaveTable(waveTable.pointer());

    }

    void WaveTableModule::prepare(Float64 rate, size_t size) {
        this->waveTable->prepare(rate, size);
    }

    void WaveTableModule::process(const AudioBuffer<Float64> &buffer) {
        this->waveTable->process(buffer);
    }

    void WaveTableModule::openWindow() {

        if(Factory::valid(window)) return;

        const Int moduleWidth = i32(wtComponent->size().x);
        const Int moduleHeight = i32(wtComponent->size().y);
        const String moduleName = "WaveTable";

        this->window = Factory::createWindow(wtComponent, RenderBackend::Best, moduleName, moduleWidth, moduleHeight);

    }

    Window *WaveTableModule::currentWindow() { return window; }

    WaveTable *WaveTableModule::processor() { return waveTable.pointer(); }

    WaveTableComponent *WaveTableModule::component() { return wtComponent; }

    WaveTableComponent::WaveTableComponent() : Component(Point::zero, { 800, 500 }) {

        setColor(Colors::transparent);

        this->positionKnob = Factory::createComponent<RotaryKnob>(Point::zero, 100.0f);
        this->positionKnob->setValue(0.0f, false);
        this->positionKnob->setDefaultValue(0.0f);
        this->positionKnob->setLabelPrecision(1);
        this->positionKnob->setLabelText("Frame");
        this->positionKnob->addListener(this);

    }

    void WaveTableComponent::draw(SkCanvas *canvas, Float64 deltaTime) {

        Component::draw(canvas, deltaTime);

    }

    void WaveTableComponent::initialize() {

        Component::initialize();
        this->addChild(positionKnob);

    }

    void WaveTableComponent::setWaveTable(WaveTable *wt) {

        this->waveTable = wt;
        this->positionKnob->setRange(Range<Float64>(1.0, f64(waveTable->maxFrames())));

    }

    void WaveTableComponent::valueChanged(RotaryKnob *knob, Float64 newValue) {

        if(!waveTable) return;

        if(knob == this->positionKnob){
            this->waveTable->setCurrentFrame(st(std::floor(newValue)) - 1);
        }

    }

}
