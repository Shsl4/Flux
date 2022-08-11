#pragma once

#include <Flux/Audio/AudioObject.h>
#include "AudioBuffer.h"
#include "Flux/UI/Components/Component.h"

namespace Flux::Audio {

    class Pipeline;

    class PipelineElement : public AudioObject {

        friend Pipeline;
        
    public:
        
        PipelineElement() = delete;

        void prepare(Float64 rate, UInt size) override;

        void signal(const Float64* buffer, UInt channel);
        
        NODISCARD bool isInPipeline(const PipelineElement* other) const;
        
        /**
         * \brief Links an output channel to an other element
         * \param fromChannel The output channel to link (from this element)
         * \param toChannel The input channel to link (to the other element)
         * \param element The element to link
         * \return Whether the link succeeded
         */
        NODISCARD bool linkOutput(UInt fromChannel, UInt toChannel, PipelineElement* element);
        
        NODISCARD bool canLinkOutput(UInt fromChannel, UInt toChannel, const PipelineElement* element) const;
        
        void unlinkOutput(UInt channel, UInt index);
        
        void unlinkInput(UInt channel);
        
        NODISCARD Pipeline* getPipeline() const;

        NODISCARD virtual SharedPointer<UserInterface::Component> getComponent() const = 0;
        
        virtual void onLink(UserInterface::Flow flow, UInt channel);
        
        virtual void onUnlink(UserInterface::Flow flow, UInt channel);

        virtual void process(AudioBuffer<Float64> const& buffer) = 0;

        virtual void discard();
    
    protected:

        PipelineElement(UInt inChannels, UInt outChannels);
    
    private:

        struct Link {
            
            Link() = default;
            
            Link(PipelineElement* const pointer, const UInt targetChannel)
                : pointer(pointer), targetChannel(targetChannel) {}
            
            PipelineElement* pointer = nullptr;
            UInt targetChannel = 0;
            
        };

        const UInt numIns;
        const UInt numOuts;

        UInt signalCount = 0;
        Pipeline* pipeline = nullptr;

        Array<Float64> stateBuffers = {};
        Array<Array<Link>> next = {};
        Array<Link> previous = {};
        
    };
    
}
