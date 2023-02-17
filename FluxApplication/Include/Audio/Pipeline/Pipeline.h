#pragma once

#include <Flux/Audio/AudioObject.h>
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
        T* addElement(Args&&... args) {

            auto element = Allocator<T>::construct(std::forward<Args>(args)...);
            this->elements += element;
            element->pipeline = this;
            element->createComponent(pipelineView);
            dynamic_cast<Node*>(element->getComponent())->setElement(element);

            if(getBufferSize() > 0) {
                element->prepare(getSampleRate(), getBufferSize());
            }

            return element;

        }

        void removeElement(PipelineElement* element);

        void setPipelineView(UserInterface::MasterView* view);

        // private:

        UInt channelCount = 0;

        UserInterface::MasterView* pipelineView;

        MutableArray<PipelineInput*> inputChannels;
        MutableArray<PipelineOutput*> outputChannels;
        SmartArray<PipelineElement> elements;

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

            FilterElement::prepare(rate, size);

            for (auto& filter : filters) {
                filter->prepare(rate, size);
            }

        }

        void createComponent(UserInterface::Compound* parent) override {
            this->component = parent->addChild<LPFNode>();
        }

        NODISCARD UserInterface::Component* getComponent() const override {
            return this->component;
        }

        void process(AudioBuffer<Float64> const& buffer) override {

            filters[0]->process(buffer.getChannelBuffer(0));
            filters[1]->process(buffer.getChannelBuffer(1));

        }

    private:

        LPFNode* component;

    };

    class HPFElement : public FilterElement {
    public:

        HPFElement();

        void createComponent(UserInterface::Compound* parent) override {
            this->component = parent->addChild<HPFNode>();
        }

        NODISCARD UserInterface::Component* getComponent() const override {
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

        HPFNode* component;

    };

}
