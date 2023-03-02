#pragma once

#include <Flux/Audio/AudioObject.h>

#include "AudioEffect.h"

namespace Flux::Audio {

    template<class ObjectType> requires std::is_base_of_v<AudioObject, ObjectType>
    class MultiProcessor : AudioEffect {
        
    public:

        MultiProcessor() = default;

        void initialize(UInt channelCount) {

            if(channelCount == 0){
                throw Exceptions::Exception("The number of channels must be greater than 0.");
            }

            destroy();
            
            for (UInt i = 0; i < channelCount; ++i) {
                this->objects += Allocator<ObjectType>::construct();
            }
            
        }

        void destroy() {

            objects.clear();
            
        }

        void prepare(Float64 rate, UInt size) override {

            AudioObject::prepare(rate, size);
            
            for(const auto& object : objects) {

                object->prepare(rate, size);
                
            }
            
        }

        bool process(Float64* buffer) override {

            Float64* pointer = buffer;
            
            for(const auto& object : objects) {

                object->process(pointer);
                pointer += getBufferSize();
                
            }

            return true;
            
        }

        void process(Float64** buffer) {

            Int channel = 0;
            
            for(const auto& object : objects) {

                object->process(buffer[channel]);
                ++channel;
                
            }
            
        }

        void invoke(Function<void(ObjectType*)> const& function) {

            for (auto* object : objects) {

                function(object);

            }
            
        }

        ObjectType* get(UInt index) const { return objects[index].raw(); }
    
    private:

        SmartArray<ObjectType> objects;
        
    };
    
}
