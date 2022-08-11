#include <Audio/Pipeline/Pipeline.h>
#include <Audio/Pipeline/PipelineElement.h>

#include "Flux/Core/Utilities/ArrayUtils.h"

namespace Flux::Audio {

    void PipelineElement::discard() {

        pipeline->removeElement(this);
                
    }

    PipelineElement::PipelineElement(UInt inChannels, UInt outChannels) : numIns(inChannels), numOuts(outChannels) {

        for (UInt i = 0; i < numIns; ++i) { previous += Link(); }

        for (UInt i = 0; i < numOuts; ++i) {
            next += Array<Link>();
            stateBuffers += Array<Float64>();
        }

    }
    
    void PipelineElement::prepare(Float64 rate, UInt size) {

        AudioObject::prepare(rate, size);

        const size_t newSize = size * sizeof(Float64) * numOuts;
        stateBuffers.resize(newSize);
        memset(stateBuffers.begin(), 0, newSize);

    }

    void PipelineElement::signal(const Float64* buffer, UInt channel) {

        ++signalCount;

        if(numOuts) {
            const size_t offset = static_cast<size_t>(channel) * getBufferSize();
            memcpy(stateBuffers.begin() + offset, buffer, sizeof(Float64) * getBufferSize());
        }

        if (signalCount >= numIns) {

            // todo: fix this
            if(numOuts) {
                for (size_t i = 0; i < numOuts; ++i) {
                    process({stateBuffers.begin(), numOuts, getBufferSize()});
                }
            }
            else {
                process({const_cast<Float64*>(buffer), 1, getBufferSize()});
            }

            UInt ch = 0;
            
            for (auto const& links : next) {
                
                for (const auto [pointer, targetChannel] : links) {
                
                    if (!pointer) { continue; }

                    const size_t offset = static_cast<size_t>(ch) * getBufferSize();
                    pointer->signal(stateBuffers.begin() + offset, targetChannel);
                
                }

                ++ch;
                
            }

            signalCount = 0;
        }

    }

    bool PipelineElement::isInPipeline(const PipelineElement* other) const {
        return other && pipeline && other->pipeline == pipeline;
    }

    bool PipelineElement::linkOutput(UInt fromChannel, UInt toChannel, PipelineElement* element) {

        if (!canLinkOutput(fromChannel, toChannel, element)) { return false; }

        element->unlinkInput(toChannel);
        
        Array<Link>& currentLinks = next[fromChannel];
        
        currentLinks += { element, toChannel };
        
        Link& otherLink = element->previous[toChannel];
        otherLink.pointer = this;
        otherLink.targetChannel = fromChannel;

        element->onLink(UserInterface::Flow::Input, toChannel);
        onLink(UserInterface::Flow::Output, fromChannel);

        return true;

    }

    bool PipelineElement::canLinkOutput(UInt fromChannel, UInt toChannel, const PipelineElement* element) const {
        return isInPipeline(element) && fromChannel < numOuts && toChannel < element->numIns;
    }
    
    void PipelineElement::unlinkOutput(UInt channel, UInt index) {

        Array<Link>& currentLinks = next[channel];

        if (index < currentLinks.getSize() && currentLinks[index].pointer) {

            const Link& currentLink = currentLinks[index];
            
            // Call unlink events
            currentLink.pointer->onUnlink(UserInterface::Flow::Input, currentLink.targetChannel);
            onUnlink(UserInterface::Flow::Output, channel);

            Link& otherLink = currentLink.pointer->previous[currentLink.targetChannel];

            // Clear references in both directions
            otherLink.pointer = nullptr;
            otherLink.targetChannel = 0;

            currentLinks.removeAt(index);
            
        }
        
    }
    
    void PipelineElement::unlinkInput(UInt channel) {

        Link& currentLink = previous[channel];

        if (currentLink.pointer) {

            // Call unlink events
            currentLink.pointer->onUnlink(UserInterface::Flow::Output, currentLink.targetChannel);
            onUnlink(UserInterface::Flow::Input, channel);

            Array<Link>& otherLinks = currentLink.pointer->next[currentLink.targetChannel];
            
            for (size_t i = 0; i < otherLinks.getSize(); ++i) {
                if(otherLinks[i].pointer == this) {
                    otherLinks.removeAt(i);
                    break;
                }
            }

            currentLink.pointer = nullptr;
            currentLink.targetChannel = 0;
            
        }
        
    }

    Pipeline* PipelineElement::getPipeline() const { return this->pipeline; }

    void PipelineElement::onLink(UserInterface::Flow flow, UInt channel) {

        Console::logStatus("Linked {}. Direction: {}, Channel: {}", getClassName(), flow == UserInterface::Flow::Input ? "input" : "output", channel);
        
    }

    void PipelineElement::onUnlink(UserInterface::Flow flow, UInt channel) {
        Console::logStatus("Unlinked {}. Direction: {}, Channel: {}", getClassName(), flow == UserInterface::Flow::Input ? "input" : "output", channel);

    }
    
}
