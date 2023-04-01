#pragma once

#include <Application/Module.h>
#include <Application/FilterComponent.h>
#include <Audio/Effects/Filters/IIRFilter.h>

namespace Flux {

    class FilterModule : public Module<Audio::IIRFilter, FilterComponent> {

    public:

        void initialize() override;

        void process(const AudioBuffer<Float64> &buffer) override;

        void prepare(Float64 rate, size_t size) override;

        void openWindow() override;

        Audio::IIRFilter* processor() override { return this->filter.pointer(); }

        Window* currentWindow() override { return Factory::valid(window) ? window : nullptr; }

        FilterComponent* component() override { return Factory::valid(moduleComponent) ? moduleComponent : nullptr; }

    private:

        Window* window = nullptr;
        FilterComponent* moduleComponent = nullptr;
        Shared<Audio::IIRFilter> filter = nullptr;

    };

}