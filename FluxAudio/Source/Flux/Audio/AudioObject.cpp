#include <AudioObject.h>

namespace Flux::Audio {

    void AudioObject::prepare(const Float64 rate, const UInt size) {
        
        fassert(rate > 0.0 && size > 0);
        
        this->sampleRate = rate;
        this->bufferSize = size;
        
    }

}

