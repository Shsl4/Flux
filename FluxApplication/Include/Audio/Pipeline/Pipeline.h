#pragma once

#include <Flux/Audio/AudioObject.h>
#include <Flux/Core/Memory/Array.h>
#include <Flux/Audio/Effects/Filters/IIRFilter.h>
#include <Flux/UI/Components/MasterView.h>

#include "FilterElement.h"

#include "DefaultElements.h"

namespace Flux::Audio {

    class Pipeline : public AudioObject {

    public:

        Pipeline();

        void setupChannels(UInt inCount);

        void prepare(Float64 rate, UInt size) override;

        void render(Float64* outBuffers);

        void render(Float64** outBuffers);

        template<typename T, typename ... Args>
        WeakPointer<T> addElement(Args&&... args) {

            auto element = SharedPointer<T>::make(std::forward<Args>(args)...);
            this->elements += element;
            element->pipeline = this;
            element->createComponent(pipelineView.raw());
            element->getComponent()->template cast<Node>()->setElement(element.raw());

            if(getBufferSize() > 0) {
                element->prepare(getSampleRate(), getBufferSize());
            }

            return element.weak();

        }

        void removeElement(PipelineElement* element);

        void setPipelineView(SharedPointer<UserInterface::MasterView> const& view);

        // private:

        UInt channelCount = 0;

        WeakPointer<UserInterface::MasterView> pipelineView;

        WeakArray<PipelineInput> inputChannels;
        WeakArray<PipelineOutput> outputChannels;
        OwnedArray<PipelineElement> elements;

        Float64* processBuffers = nullptr;

    };


    class LPFNode : public FilterNode {

    public:

        LPFNode();

    };

    class HPFNode : public FilterNode {

    public:

        HPFNode();

    };

    class LPFElement : public FilterElement {
    public:

        LPFElement();

        void prepare(Float64 rate, UInt size) override {

            PipelineElement::prepare(rate, size);

            for (auto& filter : filters) {
                filter->prepare(rate, size);
            }

        }

        void createComponent(UserInterface::Compound* parent) override {
            this->component = parent->addChild<LPFNode>();
        }

        NODISCARD WeakPointer<UserInterface::Component> getComponent() const override {
            return this->component;
        }

        void process(AudioBuffer<Float64> const& buffer) override {

            filters[0]->process(buffer.getChannelBuffer(0));
            filters[1]->process(buffer.getChannelBuffer(1));

        }

    private:

        WeakPointer<LPFNode> component;

    };

    class HPFElement : public FilterElement {
    public:

        HPFElement();

        void createComponent(UserInterface::Compound* parent) override {
            this->component = parent->addChild<HPFNode>();
        }

        NODISCARD WeakPointer<UserInterface::Component> getComponent() const override {
            return this->component;
        }
        
        void prepare(Float64 rate, UInt size) override {

            PipelineElement::prepare(rate, size);

            for (auto& filter : filters) {
                filter->prepare(rate, size);
            }

        }

        void process(AudioBuffer<Float64> const& buffer) override {

            filters[0]->process(buffer.getChannelBuffer(0));
            filters[1]->process(buffer.getChannelBuffer(1));

        }

    private:

        WeakPointer<HPFNode> component;

    };

}
