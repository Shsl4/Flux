#include <AudioObject.h>

namespace Flux::Audio {

    void AudioObject::prepare(const Float64 rate, const UInt size) {
        
        fassert(rate > 0.0 && size > 0);
        
        this->sampleRate = rate;
        this->bufferSize = size;
        
    }

    Float64 AudioObject::processSingle(Float64 xn) {
        fabort("This AudioObject does not support single sample processing.");
    }

}

