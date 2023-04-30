#include <Application/FilterModule.h>

namespace Flux {

    void FilterModule::initialize() {

        this->filter = Shared<Audio::LowPassFilter>::make();
        this->moduleComponent = Factory::createComponent<FilterComponent>();

    }

    void FilterModule::process(const AudioBuffer<Float64> &buffer) {

        this->filter->process(buffer);

    }

    void FilterModule::openWindow() {

        if(Factory::valid(window)) return;

        const Int moduleWidth = i32(moduleComponent->size().x);
        const Int moduleHeight = i32(moduleComponent->size().y);
        const String moduleName = "Filter";

        this->window = Factory::createWindow(moduleComponent, RenderBackend::Best, moduleName, moduleWidth, moduleHeight);
        this->moduleComponent->plot()->setFilter(processor());

    }

    void FilterModule::prepare(Float64 rate, size_t size) {

        this->filter->prepare(rate, i32(size));

    }
}
