#pragma once

#include <Flux/Factory.h>
#include <Audio/AudioObject.h>

namespace Flux {

    template<class ProcessorType, class ComponentType>
    requires (std::is_base_of_v<Audio::AudioObject, ProcessorType> && std::is_base_of_v<Component, ComponentType>)
    class Module {

    public:

        virtual void initialize() = 0;

        virtual void prepare(Float64 rate, size_t size) = 0;

        virtual void process(AudioBuffer<Float64> const& buffer) = 0;

        virtual void openWindow() = 0;

        virtual Window* currentWindow() = 0;

        virtual ProcessorType* processor() = 0;

        virtual ComponentType* component() = 0;

    };

}