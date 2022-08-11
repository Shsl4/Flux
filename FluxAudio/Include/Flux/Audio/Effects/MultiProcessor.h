#pragma once

#include <Flux/Core/Memory/Array.h>
#include <Flux/Audio/AudioObject.h>

#include "AudioEffect.h"

namespace Flux::Audio {

    template<class ObjectType>
    class MultiProcessor : AudioEffect {
        
    public:

        MultiProcessor(){

            static_assert(std::is_base_of_v<AudioObject, ObjectType>, "ObjectType must be an AudioObject.");

        }

        void initialize(UInt channelCount) {
            
            fassertf(channelCount > 0, "The number of channels must be greater than 0.");

            destroy();
            
            for (UInt i = 0; i < channelCount; ++i) {
                this->objects += SharedPointer<ObjectType>::make();
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

        void invoke(std::function<void(ObjectType*)> const& function) {

            for (auto& object : objects) {

                function(object.raw());

            }
            
        }

        ObjectType* get(UInt index) const { return objects[index].raw(); }
    
    private:

        OwnedArray<ObjectType> objects;
        
    };
    
}
