#pragma once

#include <Flux/Audio/AudioObject.h>
#include <Flux/Audio/Effects/Filters/IIRFilter.h>
#include <Flux/Factory.h>

#include "FilterElement.h"

#include "DefaultElements.h"

namespace Flux::Audio {
    
    using namespace Flux::UI;
    
    class Pipeline : public AudioObject {

    public:

        Pipeline() = default;

        void setupChannels(UInt inCount);

        void prepare(Float64 rate, UInt size) override;

        void render(Float64* outBuffers);

        void render(Float64** outBuffers);

        template<typename T, typename ... Args>
        T* addElement(Args&&... args) {

            auto element = Allocator<T>::construct(std::forward<Args>(args)...);
            this->elements += element;
            element->pipeline = this;
            element->createComponent(view);

            if(bufferSize() > 0) {
                element->prepare(sampleRate(), bufferSize());
            }

            return element;

        }

        void removeElement(PipelineElement* element);

        void setPipelineView(Component* view){
            this->view = view;
        }

        NODISCARD SmartArray<PipelineElement> const& getElements() const { return elements; }

        private:
        
        Component* view = nullptr;

        UInt channelCount = 0;
        
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

        void createComponent(Component* parent) override {
            this->component = Factory::createComponent<LPFNode>();
            parent->addChild(this->component);
        }

        NODISCARD Component* getComponent() const override {
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

        void createComponent(Component* parent) override {

            this->component = Factory::createComponent<HPFNode>();
            parent->addChild(this->component);

        }

        NODISCARD Component* getComponent() const override {
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
