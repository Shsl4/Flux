#include <Audio/Pipeline/Pipeline.h>
#include <skia/effects/SkDashPathEffect.h>
#include <skia/effects/SkGradientShader.h>

#include "Flux/Core/Utilities/ArrayUtils.h"

namespace Flux::Audio {

    Pipeline::Pipeline() {
    }

    void Pipeline::setupChannels(UInt inCount) {

        if(channelCount != inCount) {

            const UInt diff = std::abs(i32(channelCount) - i32(inCount));
            
            if(channelCount > inCount) {
                
                for (size_t i = 0; i < diff; ++i) {
                    inputChannels.removeAt(inputChannels.getSize() - 1);
                    outputChannels.removeAt(outputChannels.getSize() - 1);
                }
                
            }
            else {

                for (size_t i = 0; i < diff; ++i) {

                    inputChannels += addElement<PipelineInput>();
                    outputChannels += addElement<PipelineOutput>();

                }
                
            }

            channelCount = inCount;
            
        }
        
    }
    
    void Pipeline::prepare(Float64 rate, UInt size) {

        AudioObject::prepare(rate, size);

        for (size_t i = 0; i < inputChannels.getSize(); ++i) {
            
            inputChannels[i]->prepare(getSampleRate(), getBufferSize());
            outputChannels[i]->prepare(getSampleRate(), getBufferSize());
            
        }

        for (auto const& element : elements) {

            element->prepare(rate, size);
            
        }

        Allocator<Float64>::release(processBuffers);
        processBuffers = Allocator<Float64>::alloc(static_cast<size_t>(channelCount) * size);
        
    }

    void Pipeline::render(Float64* outBuffers) {

        const size_t size = sizeof(Float64) * channelCount * getBufferSize();
        memcpy(processBuffers, outBuffers, size);
        memset(outBuffers, 0, size);
        
        Float64* buffer = outBuffers;
        
        for (const auto& channel : outputChannels) {
            channel->prepareOutput(buffer);
            buffer += getBufferSize();
        }
        
        buffer = processBuffers;
        
        for (const auto& channel : inputChannels) {
            channel->signal(buffer, 0);
            buffer += getBufferSize();
        }

    }
    
    void Pipeline::render(Float64** outBuffers) {


        
    }

    void Pipeline::removeElement(PipelineElement* element) {

        for (size_t i = 0; i < elements.getSize(); ++i) {

            if(elements[i].raw() == element) {

                for (UInt channel = 0; channel < element->numIns; ++channel) {
                    element->unlinkInput(channel);
                }
                
                for (UInt channel = 0; channel < element->numOuts; ++channel) {

                    const UInt sz = static_cast<UInt>(element->next.getSize());
                    
                    for (UInt out = 0; out < sz; ++out) {
                        
                        element->unlinkOutput(channel, out);

                    }
                    
                }
                
                element->pipeline = nullptr;
                elements.removeAt(i);
                return;
                
            }
            
        }
        
    }

    void Pipeline::setPipelineView(const SharedPointer<UserInterface::MasterView> &view) {
        this->pipelineView = view.weak();
    }
    
    LPFNode::LPFNode() : FilterNode(2, 2, 25.0, 200.0, 50.0, 5.0, UserInterface::Colors::green) {}

    HPFNode::HPFNode() : FilterNode(2, 2, 25.0, 200.0, 50.0, 5.0, UserInterface::Colors::red) {}

    LPFElement::LPFElement() : FilterElement(2, 2) {

        filters += SharedPointer<LowPassFilter>::make();
        filters += SharedPointer<LowPassFilter>::make();

    }

    HPFElement::HPFElement(): FilterElement(2, 2) {

        filters += SharedPointer<HighPassFilter>::make();
        filters += SharedPointer<HighPassFilter>::make();

    }
}

