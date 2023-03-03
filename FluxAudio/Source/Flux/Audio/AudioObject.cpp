#include <AudioObject.h>

namespace Flux::Audio {

    void AudioObject::prepare(const Float64 rate, const UInt size) {
        
        assert(rate > 0.0 && size > 0);
        
        this->sr = rate;
        this->sz = size;
        
    }

}

