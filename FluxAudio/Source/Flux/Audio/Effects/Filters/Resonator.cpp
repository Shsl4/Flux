#include <Effects/Filters/Resonator.h>
#include <Flux/Core/Math/Math.h>

namespace Flux::Audio {

    Float64 Resonator::getMagnitude(Float64 omega) const {

        const std::complex<Float64> z1(std::cos(omega), -sin(omega));
        const std::complex<Float64> z2(std::cos(2.0 * omega), -sin(2.0 * omega));
        const std::complex<Float64> hNum = coefficients[a0];
        const std::complex<Float64> hDenom = (1.0 + coefficients[b1] * z1 + coefficients[b2] * z2);
        const Float64 magNum = std::sqrt(hNum.real() * hNum.real() + hNum.imag() * hNum.imag());
        const Float64 magDenom = std::sqrt(hDenom.real() * hDenom.real() + hDenom.imag() * hDenom.imag());
        const Float64 mag = magNum / magDenom;

        return mag;
        
    }

    Float64 Resonator::getArgument(Float64 omega) const {

        const std::complex<Float64> z1(std::cos(omega), -sin(omega));
        const std::complex<Float64> z2(std::cos(2.0 * omega), -sin(2.0 * omega));
        const std::complex<Float64> hNum = coefficients[a0];
        const std::complex<Float64> hDenom = (1.0 + coefficients[b1] * z1 + coefficients[b2] * z2);
        
        return atan2(hNum.imag(), hNum.real()) - atan2(hDenom.imag(), hDenom.real());
        
    }

    Float64 Resonator::processSingle(const Float64 xn) {
        
        const Float64 yn = coefficients[a0] * xn -
            coefficients[b1] * stateArray[y_z1] -
            coefficients[b2] * stateArray[y_z2];

        stateArray[y_z2] = stateArray[y_z1];
        stateArray[y_z1] = yn;

        return yn;
        
    }

    void Resonator::recalculateCoefficients() {

        const Float64 theta = 2.0 * Math::pi<Float64> * (getCutoffFrequency() / getSampleRate());
        const Float64 bandwidth = getCutoffFrequency() / getResonance();
        coefficients[b2] = std::exp(-2.0 * Math::pi<Float64> * (bandwidth / getSampleRate()));
        coefficients[b1] = ((-4.0 * coefficients[b2]) / (1.0 + coefficients[b2])) * std::cos(theta);
        coefficients[a0] = (1.0 - coefficients[b2]) * std::sqrt(1.0 - ((coefficients[b1] * coefficients[b1]) / 4.0 * coefficients[b2]));
        
    }

}
