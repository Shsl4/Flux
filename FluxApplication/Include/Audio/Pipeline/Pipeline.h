#pragma once

#include <Flux/Audio/AudioObject.h>

#include <Flux/Core/Memory/Array.h>

#include "Flux/Audio/Effects/Filters/IIRFilter.h"
#include "Flux/UI/Components/MasterView.h"

#include "PipelineElement.h"
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

        void setPipelineView(SharedPointer<UserInterface::MasterView> const& view)  { this->pipelineView = view.weak(); }

    // private:
        
        UInt channelCount = 0;

        WeakPointer<UserInterface::MasterView> pipelineView;
        
        OwnedArray<PipelineInput> inputChannels;
        OwnedArray<PipelineOutput> outputChannels;

        OwnedArray<PipelineElement> elements;
        
        Float64* processBuffers = nullptr;
        
    };

    class LPFNode : public Node {

    public:
        
        LPFNode(PipelineElement* elem) : Node(elem, 1, 1, 25.0, 150.0, 50.0, 5.0, UserInterface::Colors::green) {}
        
    };

    class LPFElement : public PipelineElement {
    public:
        
        LPFElement() : PipelineElement(1, 1) {

            component = SharedPointer<LPFNode>::make(this);
            filters += LowPassFilter();
            //filters += LowPassFilter();
            
        }

        NODISCARD SharedPointer<UserInterface::Component> getComponent() const override {
            return this->component;
        }

        void prepare(Float64 rate, UInt size) override {

            PipelineElement::prepare(rate, size);

            for (auto& filter : filters) {
                filter.prepare(rate, size);
            }
            
        }

        void process(AudioBuffer<Float64> const& buffer) override {

            filters[0].process(buffer.getChannelBuffer(0));
            //filters[1].process(buffer.getBuffer(1));
            
        }

    private:

        SharedPointer<LPFNode> component;
        Array<LowPassFilter> filters;
        
    };
    
}
