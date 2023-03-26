#include <Audio/Effects/Filters/Biquad.h>

namespace Flux::Audio {
    
    Biquad::Biquad() {

        coefficients[a0] = 1.0;
        coefficients[a1] = 0.73;
        coefficients[a2] = 1.0;
        coefficients[b1] = -0.78;
        coefficients[b2] = 0.88;

    }

    void Biquad::setParameters(BiquadParameters const& value) {
        this->params = value;
    }

    void Biquad::recalculateCoefficients() {
        
    }

    Float64 Biquad::processSingle(Float64 xn, size_t channel) {

        if(params.calcType == BiquadAlgorithm::kDirect) {

            // y[n] = a0 * x[n] + a1 * x[n - 1] + a2 * x[n - 2] - b1 * y[n - 1] - b2 * y[n - 2]
            Float64 yn = coefficients[a0] * xn +
                         coefficients[a1] * state[channel][x_z1] +
                         coefficients[a2] * state[channel][x_z2] -
                         coefficients[b1] * state[channel][y_z1] -
                         coefficients[b2] * state[channel][y_z2];
            

            checkFloatUnderflow(yn);

            state[channel][x_z2] = state[channel][x_z1];
            state[channel][x_z1] = xn;

            state[channel][y_z2] = state[channel][y_z1];
            state[channel][y_z1] = yn;

            return yn;
            
        }

        if (params.calcType == BiquadAlgorithm::kCanonical) {

            const Float64 wn = xn - coefficients[b1] * state[channel][x_z1] - coefficients[b2] * state[channel][x_z2];
            const Float64 yn = wn * coefficients[a0] + coefficients[a1] * state[channel][x_z1] + coefficients[a2] * state[channel][x_z2];

            state[channel][x_z2] = state[channel][x_z1];
            state[channel][x_z1] = wn;

            return yn;
            
        }

        if (params.calcType == BiquadAlgorithm::kTransposeDirect) {

            const Float64 wn = xn + state[channel][y_z1];
            Float64 yn = coefficients[a0] * wn + state[channel][x_z1];

            checkFloatUnderflow(yn);

            state[channel][y_z1] = state[channel][y_z2] - coefficients[b1] * wn;
            state[channel][y_z2] = -coefficients[b2] * wn;

            state[channel][x_z1] = state[channel][x_z2] + coefficients[a1] * wn;
            state[channel][x_z2] = coefficients[a2] * wn;

            return yn;
            
        }

        if(params.calcType == BiquadAlgorithm::kTransposeCanonical) {

            Float64 yn = coefficients[a0] * xn + state[channel][x_z1];

            checkFloatUnderflow(yn);

            state[channel][x_z1] = coefficients[a1] * xn - coefficients[b1] * yn + state[channel][x_z2];
            state[channel][x_z2] = coefficients[a2] * xn - coefficients[b2] * yn;

            return yn;
            
        }
        
        return 0.0;
        
    }

}

    
