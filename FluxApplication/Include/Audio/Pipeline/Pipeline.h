#pragma once

#include <Flux/Audio/AudioObject.h>

#include <Flux/Core/Memory/Array.h>

#include "Flux/Audio/Effects/Filters/IIRFilter.h"
#include "Flux/UI/Components/MasterView.h"
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
            pipelineView->addChild(element->getComponent());

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

        OwnedArray<PipelineInput> inputChannels;
        OwnedArray<PipelineOutput> outputChannels;
        OwnedArray<PipelineElement> elements;

        Float64* processBuffers = nullptr;

    };

    class FilterElement : public PipelineElement{

    public:

        FilterElement(UInt inChannels, UInt outChannels);

        NODISCARD virtual Array<IIRFilter*> getFilters() const = 0;

    };

    class LPFNode : public FilterNode {

    public:

        explicit LPFNode(FilterElement* elem);

    };

    class HPFNode : public FilterNode {

    public:

        explicit HPFNode(FilterElement* elem);

    };

    class LPFElement : public FilterElement {
    public:

        LPFElement();

        NODISCARD Array<IIRFilter*> getFilters() const override{
            Array<IIRFilter*> ar;
            for(auto& e : filters){
                ar += e.raw();
            }
            return ar;
        }

        NODISCARD SharedPointer<UserInterface::Component> getComponent() const override {
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

        SharedPointer<LPFNode> component;
        OwnedArray<LowPassFilter> filters;

    };

    class HPFElement : public FilterElement {
    public:

        HPFElement() : FilterElement(2, 2) {

            component = SharedPointer<HPFNode>::make(this);
            filters += SharedPointer<HighPassFilter>::make();
            filters += SharedPointer<HighPassFilter>::make();

        }

        NODISCARD SharedPointer<UserInterface::Component> getComponent() const override {
            return this->component;
        }

        NODISCARD Array<IIRFilter*> getFilters() const override{
            Array<IIRFilter*> ar;
            for(auto& e : filters){
                ar += e.raw();
            }
            return ar;
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

        SharedPointer<HPFNode> component;
        OwnedArray<HighPassFilter> filters;

    };

}
