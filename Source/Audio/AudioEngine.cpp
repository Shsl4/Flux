#include <Audio/AudioEngine.h>
#include <rtaudio/RtAudio.h>

namespace Flux {

    Int audioCallback(void* outBuffer, void* inBuffer, unsigned int bufferSize,
        double streamTime, RtAudioStreamStatus status, void* data) {

        static_cast<AudioEngine*>(data)->process(static_cast<Float64*>(inBuffer), static_cast<Float64*>(outBuffer));
        return 0;
        
    }

    AudioEngine::AudioEngine() {
        
        std::vector<RtAudio::Api> apis;
        RtAudio::getCompiledApi(apis);
        
        if (apis.empty() || (apis.size() == 1 && apis[0] == RtAudio::Api::RTAUDIO_DUMMY)) {
            throw Exceptions::Exception("No Audio API is available. Make sure you compiled RtAudio with at least one API.\n");
        }
        
        try{
            this->audio = Allocator<RtAudio>::construct();
        }
        catch(std::exception const&){
            throw Exceptions::Exception("Failed to initialize audio system.\n");
        }

        std::function<void(RtAudioErrorType, const std::string&)> callback = [](RtAudioErrorType err, std::string const& msg){
            Console::error("{}\n", msg.c_str());
        };
        
        audio->setErrorCallback(callback);
        
    }

    AudioEngine::~AudioEngine() {
        close();
        Allocator<RtAudio>::destroy(audio);
    }

    void AudioEngine::listDevices() const {

        const UInt count = audio->getDeviceCount();
 
        if(count == 0) {

            Console::log("There are no audio devices available.\n");
            return;
            
        }

        Console::log("Listing audio devices:\n");

        size_t index = 0;
        
        for (auto& id : audio->getDeviceIds()) {

            RtAudio::DeviceInfo info = audio->getDeviceInfo(id);

            Console::log("[{}] -> {} (In: {}, Out: {})\n", index++, info.name, info.inputChannels, info.outputChannels);
                        
        }
        
    }

    void AudioEngine::initialize(const Float64 rate, const UInt size) {

        assert(rate > 0.0);
        assert(size >= 16);
                
        this->sr = rate;
        this->bufSize = size;

        open(audio->getDefaultInputDevice(), audio->getDefaultOutputDevice());

    }

    void AudioEngine::close() {

        if(audio->isStreamOpen()) {
            audio->stopStream();
            audio->closeStream();
            closed();
        }
        
    }

    struct DeviceData {

        RtAudio::StreamParameters params;
        RtAudio::DeviceInfo info;

    };

    Optional<DeviceData> streamParameters(RtAudio* audio, UInt device, bool output){

        DeviceData data;

        // TODO: TRY / CATCH
        data.info = audio->getDeviceInfo(device);
        
        if((output && data.info.outputChannels == 0) || (!output && data.info.inputChannels == 0)) { return {}; }

        data.params.deviceId = device;
        data.params.nChannels = output ? data.info.outputChannels : data.info.inputChannels;

        return Optional<DeviceData>(data);

    }

    bool AudioEngine::open(UInt inputId, UInt outputId) {

        close();

        auto inData = streamParameters(audio, inputId, false);
        auto outData = streamParameters(audio, outputId, true);

        if(!inData && !outData) return false;

        this->outputChannels = outData ? outData->params.nChannels : 0;
        this->inputChannels = inData ? inData->params.nChannels : 0;

        RtAudio::StreamOptions options;
        options.flags = RTAUDIO_NONINTERLEAVED | RTAUDIO_MINIMIZE_LATENCY;

        this->sr = outData ? outData->info.preferredSampleRate : (inData ? inData->info.preferredSampleRate : 0);

        RtAudioErrorType err = audio->openStream(&outData->params, &inData->params, RTAUDIO_FLOAT64,
                          static_cast<UInt>(sr), &bufSize, &audioCallback, this, &options);
        
        if(err != RtAudioErrorType::RTAUDIO_NO_ERROR){
            
            this->outputChannels = 0;
            this->inputChannels = 0;
            this->sr = 0.0;

            Console::error("Failed to open audio device!\n");
            return false;
            
        }

        prepare(sr, bufSize);

        audio->startStream();

        opened();

        Console::log("AudioEngine initialized.\n");

        if (inData){
            Console::log("Input device: {}\n", audio->getDeviceInfo(inputId).name);
        }

        if (outData){
            Console::log("Output device: {}\n", audio->getDeviceInfo(outputId).name);
        }

        return true;

    }

    void AudioEngine::setSampleRate(Float64 value) {

        assert(value > 0.0);
        this->sr = value;

    }
    
    void AudioEngine::setBufferSize(UInt value) {

        assert(value >= 16);
        this->bufSize = value;

    }

    UInt AudioEngine::numOutputChannels() const { return this->outputChannels; }
    
    UInt AudioEngine::numInputChannels() const { return this->inputChannels; }
    
    Float64 AudioEngine::sampleRate() const { return this->sr; }
    
    UInt AudioEngine::bufferSize() const { return this->bufSize; }
    
    void AudioEngine::opened() { }
    
    void AudioEngine::closed() { }

}

