#include <Audio/Pipeline/Pipeline.h>
#include <Audio/Pipeline/PipelineElement.h>

namespace Flux::Audio {

    void PipelineElement::createComponent(Component* parent) {}

    void PipelineElement::discard() {

        pipeline->removeElement(this);
                
    }

    PipelineElement::PipelineElement(UInt inChannels, UInt outChannels) : numIns(inChannels), numOuts(outChannels), signalManager(inChannels) {

        for (UInt i = 0; i < numIns; ++i) { previous += Link(); }

        if(hasOutputs()){
            
            this->stateBuffers = Allocator<Float64*>::allocate(outChannels);
            
            for (size_t channel = 0; channel < numOuts; ++channel) {
                stateBuffers[channel] = nullptr;
            }
            
            for (UInt i = 0; i < numOuts; ++i) {
                next += MutableArray<Link>();
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
                stateBuffers[channel] = Allocator<Float64>::allocate(size);
            
            }
            
        }
        

    }

    void PipelineElement::signal(const Float64* buffer, UInt channel) {

        signalManager.signal(channel);

        if(hasOutputs()) {
            memcpy(stateBuffers[channel], buffer, sizeof(Float64) * bufferSize());
        }

        if (signalManager.ready()) {

            // todo: fix this
            if(hasOutputs()) {
                process({stateBuffers, numOuts, bufferSize()});
            }
            else {
                process({(Float64**)const_cast<Float64*>(buffer), 1, bufferSize()});
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
        
        MutableArray<Link>& currentLinks = next[fromChannel];
        
        currentLinks += { element, toChannel };
        
        Link& otherLink = element->previous[toChannel];
        otherLink.pointer = this;
        otherLink.targetChannel = fromChannel;
        
        element->onLink(Flow::Input, toChannel);
        onLink(Flow::Output, fromChannel);

        return true;

    }

    bool PipelineElement::canLinkOutput(UInt fromChannel, UInt toChannel, const PipelineElement* element) const {
        return isInPipeline(element) && fromChannel < numOuts && toChannel < element->numIns;
    }
    
    void PipelineElement::unlinkOutput(UInt channel, UInt index) {

        MutableArray<Link>& currentLinks = next[channel];

        if (index < currentLinks.size() && currentLinks[index].pointer) {

            const Link& currentLink = currentLinks[index];
            
            // Call unlink events
            currentLink.pointer->onUnlink(Flow::Input, currentLink.targetChannel);
            onUnlink(Flow::Output, channel);

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
            currentLink.pointer->onUnlink(Flow::Output, currentLink.targetChannel);
            onUnlink(Flow::Input, channel);

            MutableArray<Link>& otherLinks = currentLink.pointer->next[currentLink.targetChannel];
            
            for (size_t i = 0; i < otherLinks.size(); ++i) {
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

    void PipelineElement::onLink(Flow flow, UInt channel) {

        Console::log("Linked {}. Direction: {}, Channel: {}\n", Type::name(*this), flow == Flow::Input ? "input" : "output", channel);
        
    }


    void PipelineElement::onUnlink(Flow flow, UInt channel) {
        
        Console::log("Unlinked {}. Direction: {}, Channel: {}\n", Type::name(*this), flow == Flow::Input ? "input" : "output", channel);

    }

    bool PipelineElement::hasOutputs() const {
        return numOuts > 0;
    }

    bool PipelineElement::hasInputs() const {
        return numIns > 0;
    }

    
}
