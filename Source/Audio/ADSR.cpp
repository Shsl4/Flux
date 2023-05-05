#include <Audio/ADSR.h>

namespace Flux {

    void ADSR::setAttack(Float64 value) {
        setChecked(a, aIncrement, value);
    }

    void ADSR::setDecay(Float64 value) {
        setChecked(d, dIncrement, value);
    }

    void ADSR::setSustain(Float64 value) {
        setChecked(s, sIncrement, value);
    }

    void ADSR::setRelease(Float64 value) {
        setChecked(r, rIncrement, value);
    }

    void ADSR::setChecked(Float64 &variable, Float64& increment, Float64 value) {

        if(value > 0.0){
            variable = value;
            increment = 1.0 / (sampleRate() * variable);
        }

    }

    void ADSR::triggerEnvelope() {

        if (!idle()){
            stop();
        }

        this->state = State::attack;

    }

    void ADSR::releaseEnvelope() {

        if (state == State::idle) return;

        this->state = State::release;

    }

    void ADSR::stop() {

        output = 0.0;
        this->state = State::idle;

    }

    void ADSR::prepare(Float64 rate, UInt size) {

        AudioObject::prepare(rate, size);

        this->aIncrement = 1.0 / (sampleRate() * a);
        this->dIncrement = 1.0 / (sampleRate() * d);
        this->sIncrement = 1.0 / (sampleRate() * s);
        this->rIncrement = 1.0 / (sampleRate() * r);

    }


}

