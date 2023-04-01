#include <Audio/AudioPlayer.h>
#include <Factory.h>

namespace Flux {

    AudioPlayer::AudioPlayer() = default;

    void AudioPlayer::process(const AudioBuffer<Float64> &buffer) {

        if(playing){
            render(buffer, sampleRate());
        }
    
    }

    void AudioPlayer::render(const AudioBuffer<Float64> &buffer, Float64 rate) {

        const bool rev = reverses();
        const auto szHead = st(playHead);
        const auto fwdDiff = st((wavefile->samplesPerChannel - szHead) / speed / (wavefile->sampleRate / rate));
        const auto backDiff = st(szHead / speed * wavefile->sampleRate / rate);
        const size_t diff = rev * backDiff + !rev * fwdDiff;
        const size_t maxSample = Math::min(buffer.bufferSize(), diff);
        const size_t maxChannel = std::min(buffer.channels(), st(wavefile->channelCount));
        const Float64 increment = direction * speed * (wavefile->sampleRate / rate);

        for(size_t sample = 0; sample < maxSample; ++sample){

            for(size_t channel = 0; channel < maxChannel; ++channel) {
                buffer[channel][sample] = wavefile->buffers[channel][st(playHead)];
            }

            playHead += increment;

        }

        if(diff == 0 || (rev && (playHead <= f64(startSample))) || (!rev && (playHead >= f64(endSample)))) finished();

    }

    void AudioPlayer::transpose(Int32 semitones) {
    
        static const Float64 a = std::pow(2.0, 1.0 / 12.0);
        this->speed = std::pow(a, semitones);
        
    }

    void AudioPlayer::setPlayRate(Float64 rate){
        this->speed = rate;
    }

    void AudioPlayer::loadFile(String const& path){
    
        stop();
        this->wavefile = Factory::loadWaveFile(path);
        resetStartAndEnd();
        
    }

    WaveFile AudioPlayer::resample(){
    
        if(!wavefile) throw Exceptions::Exception("Trying to resample but no sample is loaded!");

        resetPlayHead();

        const size_t samplesPerChannel = f64(endSample - startSample) / speed;
        const size_t totalSamples = samplesPerChannel * wavefile->channelCount;

        auto array = MutableArray<Float64>::filled(totalSamples);
        auto buffer = AudioBuffer(array.data(), wavefile->channelCount, samplesPerChannel);

        render(buffer, f64(wavefile->sampleRate));

        MutableArray<MutableArray<Float64>> newData = MutableArray<MutableArray<Float64>>();
        
        for(size_t channel = 0; channel < wavefile->channelCount; ++channel){
            
            newData += MutableArray<Float64>::filled(samplesPerChannel);
            memcpy(newData[channel].data(), buffer[channel], sizeof(Float64) * samplesPerChannel);
            
        }
        
        return WaveFile(newData, wavefile->sampleRate);
        
    }

    void AudioPlayer::setFile(WaveFile* file) {
    
        stop();
        this->wavefile = file;
        
    }

    void AudioPlayer::play() {

        if(!wavefile) return;
        this->playing = true;

    }

    void AudioPlayer::pause() {
        this->playing = false;
    }

    void AudioPlayer::resetPlayHead() {
        this->playHead = f64(reverses() ? this->endSample - 1 : this->startSample);
    }

    void AudioPlayer::stop() {
        this->playing = false;
        resetPlayHead();
    }

    void AudioPlayer::setLooping(bool value){
        this->shouldLoop = value;
    }

    void AudioPlayer::finished(){
    
        if(looping()){
            resetPlayHead();
            return;
        }
        
        stop();
    
    }

    void AudioPlayer::setPlayHead(Float64 timeInSeconds) {
    
        Float64 sample = wavefile->sampleRate * timeInSeconds;

        if(st(sample) > endSample || st(sample) < startSample) {
            finished(); return;
        }

        this->playHead = sample;
        
    }

    void AudioPlayer::setStartTime(Float64 timeInSeconds){
    
        if(!wavefile) return;
        
        setStartSample(f64(wavefile->sampleRate) * timeInSeconds);
        
    }

    void AudioPlayer::setEndTime(Float64 timeInSeconds){
        
        if(!wavefile) return;
            
        setEndSample(f64(wavefile->sampleRate) * timeInSeconds);
        
    }

    void AudioPlayer::resetStartAndEnd(){
    
        if(!wavefile) return;
        
        this->startSample = 0;
        this->endSample = wavefile->samplesPerChannel;
        
        if(!playing){
            resetPlayHead();
        }
        
    }

    void AudioPlayer::setStartSample(size_t sample){
    
        if(!wavefile) return;

        this->startSample = sample > this->endSample ? endSample : sample;
        
        if(playHead < f64(this->startSample)){
            finished();
        }
        
    }

    void AudioPlayer::setEndSample(size_t sample){
        
        if(!wavefile) return;
        
        if(sample > wavefile->samplesPerChannel) sample = wavefile->samplesPerChannel;

        this->endSample = sample < this->startSample ? startSample : sample;
        
        if(playHead > f64(this->endSample)){
            finished();
        }
    
    }

    void AudioPlayer::setReverse(bool value){
        
        this->direction = value ? -1.0 : 1.0;
        
        if(!playing){
            resetPlayHead();
        }
        
    }

    void AudioPlayer::prepare(Float64 rate, UInt size) {
        AudioObject::prepare(rate, size);
    }

}
