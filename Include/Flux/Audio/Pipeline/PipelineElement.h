#pragma once

#include <Flux/Audio/AudioObject.h>
#include "AudioBuffer.h"
#include <Flux/UI/Component.h>

#include "Application/Socket.h"

namespace Flux{
    class LinkResolver;
}

namespace Flux::Audio {

    class Pipeline;

    class PipelineElement : public AudioObject {

        friend Pipeline;
        friend LinkResolver;

        struct SignalManager {
            
            explicit SignalManager(UInt channelCount){
            
                if(channelCount == 0) channelCount = 1;
                
                signals = MutableArray<bool>(channelCount);
                
                for (size_t i = 0; i < channelCount; ++i) {
                    signals += false;
                }
                
            }
            
            FORCEINLINE void signal(UInt channel){
                
                signals[channel] = true;
                
            }
            
            NODISCARD FORCEINLINE bool ready() const{
            
                for(const auto& state : signals){
                    if(!state) return false;
                }
                
                return true;
                
            }
            
            FORCEINLINE void reset() const {
                memset(signals.begin().get(), 0, sizeof(bool) * signals.capacity());
            }

            MutableArray<bool> signals;
            
        };
        
        struct Link {
            
            Link() = default;
                
            Link(PipelineElement* const pointer, const UInt targetChannel)
                : pointer(pointer), targetChannel(targetChannel) {}

            bool operator==(Link const& other) const = default;

            PipelineElement* pointer = nullptr;
            UInt targetChannel = 0;

        };
        
    public:
        
        PipelineElement() = delete;

        void prepare(Float64 rate, UInt size) override;
        
        void signal(const Float64* buffer, UInt channel);
        
        ~PipelineElement() override;
                
        /**
         * \brief Links an output channel to an other element
         * \param[in] fromChannel The output channel to link (from this element)
         * \param[in] toChannel The input channel to link (to the other element)
         * \param[in] element The element to link
         * \return Whether the link succeeded
         */
        NODISCARD bool linkOutput(UInt fromChannel, UInt toChannel, PipelineElement* element);
        
        NODISCARD bool canLinkOutput(UInt fromChannel, UInt toChannel, const PipelineElement* element) const;
        
        NODISCARD Pipeline* getPipeline() const;
        
        NODISCARD bool isInPipeline(const PipelineElement* other) const;
        
        NODISCARD FORCEINLINE bool hasOutputs() const;
        
        NODISCARD FORCEINLINE bool hasInputs() const;
        
        void unlinkOutput(UInt channel, UInt index);
        
        void unlinkInput(UInt channel);
        
        virtual void onLink(Flow flow, UInt channel);
        
        virtual void onUnlink(Flow flow, UInt channel);

        virtual void process(AudioBuffer<Float64> const& buffer) = 0;

        virtual void createComponent(Component* parent) = 0;
        
        NODISCARD virtual Component* getComponent() const = 0;

        virtual void discard();
    
    protected:

        PipelineElement(UInt inChannels, UInt outChannels);
    
    private:

        const UInt numIns;
        const UInt numOuts;

        SignalManager signalManager;
        
        Pipeline* pipeline = nullptr;

        Float64** stateBuffers = nullptr;

        MutableArray<MutableArray<Link>> next = {};
        MutableArray<Link> previous = {};
        
    };
    
}
