#include <Audio/Effects/Filters/SAResonator.h>

namespace Flux::Audio {

    Float64 SAResonator::magnitude(Float64 omega) const {

        const std::complex<Float64> z1(std::cos(omega), -sin(omega));
        const std::complex<Float64> z2(std::cos(2.0 * omega), -sin(2.0 * omega));
        const std::complex<Float64> hNum = coefficients[a0] + coefficients[a2] * z2;
        const std::complex<Float64> hDenom = 1.0 + coefficients[b1] * z1 + coefficients[b2] * z2;
        const Float64 magNum = std::sqrt(hNum.real() * hNum.real() + hNum.imag() * hNum.imag());
        const Float64 magDenom = std::sqrt(hDenom.real() * hDenom.real() + hDenom.imag() * hDenom.imag());
        const Float64 mag = magNum / magDenom;
        
        return mag;
        
    }

    Float64 SAResonator::argument(Float64 omega) const {

        const std::complex<Float64> z1(std::cos(omega), -sin(omega));
        const std::complex<Float64> z2(std::cos(2.0 * omega), -sin(2.0 * omega));
        const std::complex<Float64> hNum = coefficients[a0] + coefficients[a2] * z2;
        const std::complex<Float64> hDenom = 1.0 + coefficients[b1] * z1 + coefficients[b2] * z2;

        return atan2(hNum.imag(), hNum.real()) - atan2(hDenom.imag(), hDenom.real());
        
    }

    Float64 SAResonator::processSingle(Float64 xn, size_t channel) {

        Float64 yn = coefficients[a0] * xn +
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
    
    void SAResonator::recalculateCoefficients() {

        const Float64 theta = 2.0 * Math::pi<Float64> * (cutoff() / sampleRate());
        const Float64 bandwidth = cutoff() / resonance();

        coefficients[b2] = std::exp(-2.0 * Math::pi<Float64> * (bandwidth / sampleRate()));
        coefficients[b1] = (-4.0 * coefficients[b2]) / (1.0 + coefficients[b2]) * std::cos(theta);
        coefficients[a0] = 1.0 - std::sqrt(coefficients[b2]);
        coefficients[a2] = -coefficients[a0];
        
    }

    
}
