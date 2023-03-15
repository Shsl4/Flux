#include <Audio/Effects/Filters/Filter.h>
#include <memory>

namespace Flux::Audio {

    void Filter::prepare(Float64 rate, UInt size) {

        AudioObject::prepare(rate, size);
        recalculateCoefficients();
        
    }

    bool Filter::process(Float64* buffer) {
        
        for (UInt sample = 0; sample < bufferSize(); ++sample) {
            buffer[sample] = processSingle(buffer[sample]);
        }
        
        return true;
        
    }

    Float64 Filter::magnitude(Float64 omega) const {
        return 0.0;
    }

    Float64 Filter::argument(Float64 omega) const {
        return 0.0;
    }

    void Filter::setCutoffFrequency(const Float64 cutoff) {

        assert(cutoff >= 0.0);
        this->cutoffFrequency = cutoff;
        recalculateCoefficients();
        
    }

    void Filter::setResonance(const Float64 value) {
        
        assert(value >= 0.0);
        this->q = value;
        recalculateCoefficients();
        
    }
    
}
