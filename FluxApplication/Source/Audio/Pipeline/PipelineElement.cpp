#include <Audio/Pipeline/Pipeline.h>
#include <Audio/Pipeline/PipelineElement.h>

#include "Flux/Core/Utilities/ArrayUtils.h"

namespace Flux::Audio {

    void PipelineElement::discard() {

        pipeline->removeElement(this);
                
    }

    PipelineElement::PipelineElement(UInt inChannels, UInt outChannels) : numIns(inChannels), numOuts(outChannels), signalManager(inChannels) {

        for (UInt i = 0; i < numIns; ++i) { previous += Link(); }

        if(hasOutputs()){
            
            this->stateBuffers = Allocator<Float64*>::alloc(outChannels);
            
            for (size_t channel = 0; channel < numOuts; ++channel) {
                stateBuffers[channel] = nullptr;
            }
            
            for (UInt i = 0; i < numOuts; ++i) {
                next += Array<Link>();
            }
        
        }

    }

    PipelineElement::~PipelineElement(){
    
        if (hasOutputs()) {
            
            for (size_t channel = 0; channel < numOuts; ++channel) {
                Allocator<Float64>::release(stateBuffers[channel]);
            }
            
            Allocator<Float64*>::release(stateBuffers);
            
        }
    
    }

    void PipelineElement::prepare(Float64 rate, UInt size) {

        AudioObject::prepare(rate, size);

        const size_t newSize = size * sizeof(Float64) * numOuts;

        if (hasOutputs()) {
            
            for (size_t channel = 0; channel < numOuts; ++channel) {
                
                Allocator<Float64>::release(stateBuffers[channel]);
                stateBuffers[channel] = Allocator<Float64>::alloc(size);
            
            }
            
        }
        

    }

    void PipelineElement::signal(const Float64* buffer, UInt channel) {

        signalManager.signal(channel);

        if(hasOutputs()) {
            memcpy(stateBuffers[channel], buffer, sizeof(Float64) * getBufferSize());
        }

        if (signalManager.ready()) {

            // todo: fix this
            if(hasOutputs()) {
                process({stateBuffers, numOuts, getBufferSize()});
            }
            else {
                process({(Float64**)const_cast<Float64*>(buffer), 1, getBufferSize()});
            }

            UInt ch = 0;
            
            for (auto const& links : next) {
                
                for (const auto [pointer, targetChannel] : links) {
                
                    if (!pointer) { continue; }

                    pointer->signal(stateBuffers[ch], targetChannel);
                
                }

                ++ch;
                
            }

            signalManager.reset();
            
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
                if (otherLinks[i].targetChannel == channel && otherLinks[i].pointer == this) {
                    otherLinks.removeAt(i);
                    break;
                }
            }

            currentLink.pointer = nullptr;
            currentLink.targetChannel = -1;
                        
        }
        
    }

    Pipeline* PipelineElement::getPipeline() const { return this->pipeline; }

    void PipelineElement::onLink(UserInterface::Flow flow, UInt channel) {

        Console::logStatus("Linked {}. Direction: {}, Channel: {}", getClassName(), flow == UserInterface::Flow::Input ? "input" : "output", channel);
        
    }


    void PipelineElement::onUnlink(UserInterface::Flow flow, UInt channel) {
        
        Console::logStatus("Unlinked {}. Direction: {}, Channel: {}", getClassName(), flow == UserInterface::Flow::Input ? "input" : "output", channel);

    }

    bool PipelineElement::hasOutputs() const {
        return numOuts > 0;
    }

    bool PipelineElement::hasInputs() const {
        return numIns > 0;
    }

    
}
