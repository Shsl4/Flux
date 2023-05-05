#pragma once

#include <Audio/AudioObject.h>

namespace Flux {

    class AudioProcessor : public Audio:: AudioObject {

    public:

        virtual void process(AudioBuffer<Float64> const& buffer) = 0;

    };

}