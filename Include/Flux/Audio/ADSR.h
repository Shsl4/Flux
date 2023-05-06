#pragma once

#include <Audio/AudioObject.h>

namespace Flux {

    class ADSR : public Audio::AudioObject {

        enum class State {

            idle,
            attack,
            decay,
            sustain,
            release

        };

    public:

        NODISCARD FORCEINLINE Float64 tick();

        void triggerEnvelope();

        void releaseEnvelope();

        void stop();

        void prepare(Float64 rate, UInt size) override;

        void setAttack(Float64 value);

        void setDecay(Float64 value);

        void setSustain(Float64 value);

        void setRelease(Float64 value);

        NODISCARD FORCEINLINE bool idle() const { return state == State::idle; }

        NODISCARD FORCEINLINE Float64 attack() const { return this->a; }

        NODISCARD FORCEINLINE Float64 decay() const { return this->d; }

        NODISCARD FORCEINLINE Float64 sustain() const { return this->s; }

        NODISCARD FORCEINLINE Float64 release() const { return this->r; }

    private:

        void setChecked(Float64& variable, Float64& increment, Float64 value);

        Float64 a = 0.005;
        Float64 d = 0.5;
        Float64 s = 1.0;
        Float64 r = 0.1;

        Float64 aIncrement = 0.0;
        Float64 dIncrement = 0.0;
        Float64 sIncrement = 0.0;
        Float64 rIncrement = 0.0;

        Float64 output = 0.0;
        State state = State::idle;

    };

    Float64 ADSR::tick() {

        switch (state) {

            case State::idle:
                return 0.0;

            case State::attack:

                output += aIncrement;

                if(output >= 1.0) {
                    output = 1.0;
                    state = sustain() == 1.0 ? State::sustain : State::decay;
                }

                break;

            case State::decay:

                output -= dIncrement;

                if (output <= s) {

                    output = s;
                    state = State::sustain;
                    
                }

                break;

        case State::sustain:
            
                break;

            case State::release:

                output -= rIncrement;

                if (output <= 0.0) stop();

                break;

        }

        return output;

    }

}