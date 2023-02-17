#include <Effects/Filters/Filter.h>
#include <memory>

namespace Flux::Audio {

    void Filter::prepare(Float64 rate, UInt size) {

        AudioObject::prepare(rate, size);
        recalculateCoefficients();
        
    }

    bool Filter::process(Float64* buffer) {
        
        for (UInt sample = 0; sample < getBufferSize(); ++sample) {
            buffer[sample] = processSingle(buffer[sample]);
        }
        
        return true;
        
    }

    Float64 Filter::getMagnitude(Float64 omega) const {
        return 0.0;
    }

    Float64 Filter::getArgument(Float64 omega) const {
        return 0.0;
    }

    void Filter::setCutoffFrequency(const Float64 cutoff) {

        assert(cutoff >= 0.0);
        this->cutoffFrequency = cutoff;
        recalculateCoefficients();
        
    }

    void Filter::setResonance(const Float64 value) {
        
        assert(value >= 0.0);
        this->resonance = value;
        recalculateCoefficients();
        
    }
    
}
