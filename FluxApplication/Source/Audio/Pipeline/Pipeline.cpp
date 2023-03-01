#include <Audio/Pipeline/Pipeline.h>
#include <skia/effects/SkDashPathEffect.h>
#include <skia/effects/SkGradientShader.h>

namespace Flux::Audio {
    
    void Pipeline::setupChannels(UInt inCount) {

        if(channelCount != inCount) {

            const UInt diff = std::abs(i32(channelCount) - i32(inCount));
            
            if(channelCount > inCount) {
                
                for (size_t i = 0; i < diff; ++i) {
                    inputChannels.removeAt(inputChannels.size() - 1);
                    outputChannels.removeAt(outputChannels.size() - 1);
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

        for (size_t i = 0; i < inputChannels.size(); ++i) {
            
            inputChannels[i]->prepare(getSampleRate(), getBufferSize());
            outputChannels[i]->prepare(getSampleRate(), getBufferSize());
            
        }

        for (auto const& element : elements) {

            element->prepare(rate, size);
            
        }

        Allocator<Float64>::release(processBuffers);
        processBuffers = Allocator<Float64>::allocate(static_cast<size_t>(channelCount) * size);
        
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
        
        for (auto* channel : inputChannels) {
            channel->signal(buffer, 0);
            buffer += getBufferSize();
        }

    }
    
    void Pipeline::render(Float64** outBuffers) {


        
    }

    void Pipeline::removeElement(PipelineElement* element) {

        for (size_t i = 0; i < elements.size(); ++i) {

            if(elements[i] == element) {

                for (UInt channel = 0; channel < element->numIns; ++channel) {
                    element->unlinkInput(channel);
                }
                
                for (UInt channel = 0; channel < element->numOuts; ++channel) {

                    const UInt sz = static_cast<UInt>(element->next.size());
                    
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
    
    
    LPFNode::LPFNode() : FilterNode(2, 2, 25.0, 200.0, 50.0, 5.0, Colors::green) {}

    HPFNode::HPFNode() : FilterNode(2, 2, 25.0, 200.0, 50.0, 5.0, Colors::red) {}

    LPFElement::LPFElement() : FilterElement(2, 2) {

        filters += Allocator<LowPassFilter>::construct();
        filters += Allocator<LowPassFilter>::construct();

    }

    HPFElement::HPFElement(): FilterElement(2, 2) {

        filters += Allocator<HighPassFilter>::construct();
        filters += Allocator<HighPassFilter>::construct();

    }
}

