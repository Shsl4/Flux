#include <Effects/Filters/Filter.h>
#include <memory>

namespace Flux::Audio {

    void Filter::prepare(Float64 rate, UInt size) {

        AudioObject::prepare(rate, size);
        recalculateCoefficients();
        
    }

    void Filter::process(Float64* buffer) {
        for (UInt sample = 0; sample < getBufferSize(); ++sample) {
            buffer[sample] = processSingle(buffer[sample]);
        }
    }

    void Filter::setCutoffFrequency(const Float64 cutoff) {

        fassert(cutoff >= 0.0);
        this->cutoffFrequency = cutoff;
        recalculateCoefficients();
        
    }

    void Filter::setResonance(const Float64 value) {
        
        fassert(value >= 0.0);
        this->resonance = value;
        recalculateCoefficients();
        
    }
    
}
