#include <Audio/Effects/Filters/Filter.h>

namespace Flux::Audio {

    void Filter::prepare(Float64 rate, UInt size) {

        state.clear();

        for (size_t i = 0; i < channelCount(); ++i) {
            
            MutableArray<Float64> arr(stateCount);
            
            for (size_t j = 0; j < stateCount; ++j) { arr += 0.0; }
            
            state += arr;
            
        }
        
        AudioObject::prepare(rate, size);
        
        recalculateCoefficients();
        
    }

    void Filter::process(AudioBuffer<Float64> const& buffer) {

        assert(channels <= buffer.channels());
        
        for(size_t channel = 0; channel < channels; ++channel) {
            
            for (UInt sample = 0; sample < bufferSize(); ++sample) {
                
                buffer[channel][sample] = processSingle(buffer[channel][sample], channel);
                
            }
            
        }
        
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
