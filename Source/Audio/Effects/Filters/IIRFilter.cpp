#include <Audio/Effects/Filters/IIRFilter.h>

namespace Flux::Audio {

    Float64 IIRFilter::magnitude(Float64 omega) const {
        
        const std::complex<Float64> z1(std::cos(omega), -sin(omega));
        const std::complex<Float64> z2(std::cos(2.0 * omega), -sin(2.0 * omega));
        const std::complex<Float64> hNum = coefficients[a0] + coefficients[a1] * z1 + coefficients[a2] * z2;
        const std::complex<Float64> hDenom = (1.0 + coefficients[b1] * z1 + coefficients[b2] * z2);
        const Float64 magNum = std::sqrt(hNum.real() * hNum.real() + hNum.imag() * hNum.imag());
        const Float64 magDenom = std::sqrt(hDenom.real() * hDenom.real() + hDenom.imag() * hDenom.imag());

        return magNum / magDenom;
        
    }

    Float64 IIRFilter::argument(Float64 omega) const {

        const std::complex<Float64> z1(std::cos(omega), -sin(omega));
        const std::complex<Float64> z2(std::cos(2.0 * omega), -sin(2.0 * omega));
        const std::complex<Float64> hNum = coefficients[a0] + coefficients[a1] * z1 + coefficients[a2] * z2;
        const std::complex<Float64> hDenom = (1.0 + coefficients[b1] * z1 + coefficients[b2] * z2);

        return atan2(hNum.imag(), hNum.real()) - atan2(hDenom.imag(), hDenom.real());
        
    }

    Float64 IIRFilter::processSingle(const Float64 xn) {
        
        const Float64& wetMix = this->filterMix;
        const Float64& dryMix = 1.0 - this->filterMix;

        const Float64 compound = wetMix * (coefficients[a0] * xn +
                                           coefficients[a1] * state[x_z1] +
                                           coefficients[a2] * state[x_z2] -
                                           coefficients[b1] * state[y_z1] -
                                           coefficients[b2] * state[y_z2]);
        
        const Float64 yn = dryMix * xn + compound;

        state[x_z2] = state[x_z1];
        state[x_z1] = xn;
        state[y_z2] = state[y_z1];
        state[y_z1] = yn;
        
        return yn;
        
    }

    bool IIRFilter::process(Float64* buffer) {

        for (UInt sample = 0; sample < bufferSize(); ++sample) {

            const Float64 xn = buffer[sample];
            
            const Float64& wetMix = this->filterMix;
            const Float64& dryMix = 1.0 - this->filterMix;

            const Float64 compound = wetMix * (coefficients[a0] * xn +
                                               coefficients[a1] * state[x_z1] +
                                               coefficients[a2] * state[x_z2] -
                                               coefficients[b1] * state[y_z1] -
                                               coefficients[b2] * state[y_z2]);
        
            const Float64 yn = dryMix * xn + compound;

            state[x_z2] = state[x_z1];
            state[x_z1] = xn;
            state[y_z2] = state[y_z1];
            state[y_z1] = yn;

            buffer[sample] = yn;
            
        }

        return true;
        
    }

    void IIRFilter::setMix(Float64 value) {
        
        this->filterMix = Math::clamp(value, 0.0, 1.0);
        
    }
    
    void LowPassFilter::recalculateCoefficients() {

        const Float64 theta = 2.0 * Math::pi<Float64> * (cutoff() / sampleRate());
        const Float64 d = 1.0 / resonance();
        const Float64 bs = (d / 2.0) * std::sin(theta);
        const Float64 beta = 0.5 * ((1.0 - bs) / (1.0 + bs));
        const Float64 gamma = (0.5 + beta) * std::cos(theta);

        const Float64 compound = 0.5 + beta - gamma;
        
        coefficients[a0] = compound / 2.0;
        coefficients[a1] = compound;
        coefficients[a2] = coefficients[a0];
        coefficients[b1] = -2.0 * gamma;
        coefficients[b2] = 2.0 * beta;        
        
    }

    void HighPassFilter::recalculateCoefficients() {

        const Float64 theta = 2.0 * Math::pi<Float64> * (cutoff() / sampleRate());
        const Float64 d = 1.0 / resonance();
        const Float64 bs = (d / 2.0) * std::sin(theta);
        const Float64 beta = 0.5 * ((1.0 - bs) / (1.0 + bs));
        const Float64 gamma = (0.5 + beta) * std::cos(theta);

        const Float64 compound = 0.5 + beta + gamma;
        
        coefficients[a0] = compound / 2.0;
        coefficients[a1] = -compound;
        coefficients[a2] = coefficients[a0];
        coefficients[b1] = -2.0 * gamma;
        coefficients[b2] = 2.0 * beta;    
        
    }

    void BandPassFilter::recalculateCoefficients() {

        const Float64 k = tan((Math::pi<Float64> * cutoff()) / sampleRate());
        const Float64 delta = k * k * resonance() + k + resonance();

        coefficients[a0] = k / delta;
        coefficients[a1] = 0.0;
        coefficients[a2] = -coefficients[a0];
        coefficients[b1] = (2.0 * resonance() * (k * k - 1.0)) / delta;
        coefficients[b2] = (k * k * resonance() - k + resonance()) / delta;
        
    }

    void NotchFilter::recalculateCoefficients() {

        const Float64 k = tan((Math::pi<Float64> * cutoff()) / sampleRate());
        const Float64 delta = k * k * resonance() + k + resonance();

        coefficients[a0] = (resonance() * (k * k + 1.0)) / delta;
        coefficients[a1] = (2.0 * resonance() * (k * k - 1.0)) / delta;
        coefficients[a2] = coefficients[a0];
        coefficients[b1] = coefficients[a1];
        coefficients[b2] = (k * k * resonance() - k + resonance()) / delta;
        
    }

    void BWLowPassFilter::recalculateCoefficients() {

        const Float64 c = 1.0 / tan((Math::pi<Float64> * cutoff()) / sampleRate());

        coefficients[a0] = 1.0 / (1.0 + std::sqrt(2.0) * c + (c * c));
        coefficients[a1] = 2.0 * coefficients[a0];
        coefficients[a2] = coefficients[a0];
        coefficients[b1] = 2.0 * coefficients[a0] * (1.0 - c * c);
        coefficients[b2] = coefficients[a0] * (1.0 - std::sqrt(2.0) * c + c * c);
        
    }

    void BWHighPassFilter::recalculateCoefficients() {

        const Float64 c = tan((Math::pi<Float64> * cutoff()) / sampleRate());

        coefficients[a0] = 1.0 / (1.0 + std::sqrt(2.0) * c + (c * c));
        coefficients[a1] = -2.0 * coefficients[a0];
        coefficients[a2] = coefficients[a0];
        coefficients[b1] = 2.0 * coefficients[a0] * ((c * c) - 1.0);
        coefficients[b2] = coefficients[a0] * (1.0 - std::sqrt(2.0) * c + c * c);
        
    }

    void BWBandPassFilter::recalculateCoefficients() {

        const Float64 bandwidth = cutoff() / resonance();
        const Float64 c = 1.0 / (std::tan((Math::pi<Float64> * cutoff() * bandwidth) / sampleRate()));
        const Float64 d = 2.0* std::cos((2.0 * Math::pi<Float64> * cutoff()) / sampleRate());

        coefficients[a0] = 1.0 / (1.0 + c);
        coefficients[a1] = 0.0;
        coefficients[a2] = -coefficients[a0];
        coefficients[b1] = -coefficients[a0] * c * d;
        coefficients[b2] = coefficients[a0] * (c - 1.0);
        
    }

    void BWNotchFilter::recalculateCoefficients() {

        const Float64 bandwidth = cutoff() / resonance();
        const Float64 c = std::tan((Math::pi<Float64> * cutoff() * bandwidth) / sampleRate());
        const Float64 d = 2.0* std::cos((2.0 * Math::pi<Float64> * cutoff()) / sampleRate());

        coefficients[a0] = 1.0 / (1.0 + c);
        coefficients[a1] = -coefficients[a0] * d;
        coefficients[a2] = coefficients[a0];
        coefficients[b1] = -coefficients[a0] * d;
        coefficients[b2] = coefficients[a0] * (1.0 - c);
        
    }

    void AllpassFilter::recalculateCoefficients() {

        const Float64 bandwidth = cutoff() / resonance();
        const Float64 compound = std::tan((bandwidth * Math::pi<Float64>) / sampleRate());
        const Float64 alpha = (compound - 1.0) / (compound + 1.0);
        const Float64 beta = -std::cos((2.0 * Math::pi<Float64> * cutoff()) / sampleRate());

        coefficients[a0] = -alpha;
        coefficients[a1] = beta * (1.0 - alpha);
        coefficients[a2] = 1.0;
        coefficients[b1] = coefficients[a1];
        coefficients[b2] = -alpha;
        
    }

}
