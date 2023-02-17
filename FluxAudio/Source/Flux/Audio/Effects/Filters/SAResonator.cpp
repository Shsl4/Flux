#include <Effects/Filters/SAResonator.h>

namespace Flux::Audio {

    Float64 SAResonator::getMagnitude(Float64 omega) const {

        const std::complex<Float64> z1(std::cos(omega), -sin(omega));
        const std::complex<Float64> z2(std::cos(2.0 * omega), -sin(2.0 * omega));
        const std::complex<Float64> hNum = coefficients[a0] + coefficients[a2] * z2;
        const std::complex<Float64> hDenom = 1.0 + coefficients[b1] * z1 + coefficients[b2] * z2;
        const Float64 magNum = std::sqrt(hNum.real() * hNum.real() + hNum.imag() * hNum.imag());
        const Float64 magDenom = std::sqrt(hDenom.real() * hDenom.real() + hDenom.imag() * hDenom.imag());
        const Float64 mag = magNum / magDenom;
        
        return mag;
        
    }

    Float64 SAResonator::getArgument(Float64 omega) const {

        const std::complex<Float64> z1(std::cos(omega), -sin(omega));
        const std::complex<Float64> z2(std::cos(2.0 * omega), -sin(2.0 * omega));
        const std::complex<Float64> hNum = coefficients[a0] + coefficients[a2] * z2;
        const std::complex<Float64> hDenom = 1.0 + coefficients[b1] * z1 + coefficients[b2] * z2;

        return atan2(hNum.imag(), hNum.real()) - atan2(hDenom.imag(), hDenom.real());
        
    }

    Float64 SAResonator::processSingle(Float64 xn) {

        Float64 yn = coefficients[a0] * xn +
            coefficients[a2] * stateArray[x_z2] -
            coefficients[b1] * stateArray[y_z1] -
            coefficients[b2] * stateArray[y_z2];

        checkFloatUnderflow(yn);

        stateArray[x_z2] = stateArray[x_z1];
        stateArray[x_z1] = xn;
        stateArray[y_z2] = stateArray[y_z1];
        stateArray[y_z1] = yn;
        
        return yn;
        
    }
    
    void SAResonator::recalculateCoefficients() {

        const Float64 theta = 2.0 * Math::pi<Float64> * (getCutoffFrequency() / getSampleRate());
        const Float64 bandwidth = getCutoffFrequency() / getResonance();

        coefficients[b2] = std::exp(-2.0 * Math::pi<Float64> * (bandwidth / getSampleRate()));
        coefficients[b1] = (-4.0 * coefficients[b2]) / (1.0 + coefficients[b2]) * std::cos(theta);
        coefficients[a0] = 1.0 - std::sqrt(coefficients[b2]);
        coefficients[a2] = -coefficients[a0];
        
    }

    
}
