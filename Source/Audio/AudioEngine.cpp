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

        const std::function callback = [](RtAudioErrorType err, std::string const& msg){
            Console::error("{}\n", msg.c_str());
        };
        
        audio->setErrorCallback(callback);
        
    }

    AudioEngine::~AudioEngine() {
        close();
        Allocator<RtAudio>::destroy(audio);
    }

    void AudioEngine::initialize() {
                
        this->bufSize = 256;

        this->inDevice = AudioDevice(audio, audio->getDefaultInputDevice());
        this->outDevice = AudioDevice(audio, audio->getDefaultOutputDevice());

        this->sr = findBestSampleRate();

        open();

    }

    void AudioEngine::close() {

        if(audio->isStreamOpen()) {

            if(audio->isStreamRunning()){
                audio->stopStream();
            }

            audio->closeStream();

            closed();

        }

    }

    void AudioEngine::addListener(Listener* listener) {
        if(listener && !listeners.contains(listener)) { this->listeners += listener; }
    }
    
    void AudioEngine::removeListener(Listener* listener) {
        listeners.removeAll(listener);
    }

    MutableArray<UInt> AudioEngine::supportedSampleRates() const {
        
        if (inDevice.valid() && outDevice.valid()) {
            return inDevice.sampleRates().intersect(outDevice.sampleRates());
        }
        
        return inDevice.valid() ? inDevice.sampleRates() : outDevice.sampleRates();
        
    }

    MutableArray<UInt> AudioEngine::supportedBufferSizes() {

        return { 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 };
        
    }

    void AudioEngine::setSampleRate(const UInt value) {

        assert(value > 0.0);

        if(!audio->isStreamOpen()) { this->sr = value; return; }

        if(supportedSampleRates().contains(value)) {

            this->sr = value;
            
            for (const auto& listener : listeners) {
                listener->sampleRateChanged(sr);
            }
            
            open();

            return;
            
        }

        throw Exceptions::Exception("Trying to set an unsupported sample rate.");

    }
    
    void AudioEngine::setBufferSize(const UInt value) {

        assert(value >= 16 && value % 2 == 0);
        this->bufSize = value;
        open();
        
    }

    void AudioEngine::setInputDevice(AudioDevice const& dev) {

        this->inDevice = dev;
        this->sr = findBestSampleRate();
        open();
        
    }
    
    void AudioEngine::setOutputDevice(AudioDevice const& dev) {
        
        this->outDevice = dev;
        this->sr = findBestSampleRate();
        open();
        
    }

    void AudioEngine::writeConfig() {

        config.favoriteApi = audio->getCurrentApi();
        config.favoriteInput = inDevice.name();
        config.favoriteOutput = outDevice.name();
        config.favoriteSampleRate = st(sr);
        config.favoriteBufferSize = bufSize;

        Json json;
        Serializer<AudioConfig>::serialize(json.archive().root(), config);
        json.write("config.json");
        
    }

    bool AudioEngine::open() {

        close();

        if(!inDevice.valid() && !outDevice.valid()) return false;
        
        RtAudio::StreamOptions options = { RTAUDIO_NONINTERLEAVED, 0,
            "AudioStream", 0};
        
        auto outParams = outDevice.outParams();
        auto inParams = inDevice.inParams();

        if(audio->openStream(outParams.nChannels > 0 ? &outParams : nullptr,
            inParams.nChannels > 0 ? &inParams : nullptr,
            RTAUDIO_FLOAT64, sr, &bufSize, &audioCallback, this, &options)){
            
            Console::error("Failed to open audio device!\n");
            return false;
            
        }
        
        prepare(f64(sr), bufSize);

        audio->startStream();
        
        opened();

        for (const auto& listener : listeners) {
            listener->audioDeviceOpened(inDevice, outDevice);
        }

        writeConfig();

        Console::log("AudioEngine initialized using {} with {} sample rate and buffer size {}.\n",
            RtAudio::getApiDisplayName(api()), sampleRate(), bufferSize());

        Console::log("Latency: {}\n", audio->getStreamLatency());
        
        if (inDevice.valid()) { Console::log("Input device: {}\n", inDevice.name()); }

        if (outDevice.valid()) { Console::log("Output device: {}\n", outDevice.name()); }
        
        return true;
        
    }

    RtAudio::Api AudioEngine::api() const { return audio->getCurrentApi(); }
    
    UInt AudioEngine::numOutputChannels() const { return this->outDevice.outChannels(); }
    
    UInt AudioEngine::numInputChannels() const { return this->inDevice.inChannels(); }

    UInt AudioEngine::sampleRate() const { return sr; }
    
    UInt AudioEngine::bufferSize() const { return this->bufSize; }
    
    void AudioEngine::setApi(RtAudio::Api value) {
        
        if(api() != value && apiSupported(value)) {

            close();

            Allocator<RtAudio>::destroy(this->audio);

            this->audio = Allocator<RtAudio>::construct(value);

            inDevice = AudioDevice(audio, audio->getDefaultInputDevice());
            outDevice = AudioDevice(audio, audio->getDefaultOutputDevice());

            this->sr = findBestSampleRate();

            open();
            
        }
        
    }

    MutableArray<AudioDevice> AudioEngine::availableInputDevices() const {

        MutableArray<AudioDevice> devices;

        for (auto const& id : audio->getDeviceIds()) {
            if(audio->getDeviceInfo(id).inputChannels > 0) {
                devices += AudioDevice(audio, id);
            }
        }

        return devices;
        
    }

    MutableArray<AudioDevice> AudioEngine::availableOutputDevices() const {

        MutableArray<AudioDevice> devices;

        for (auto const& id : audio->getDeviceIds()) {
            if(audio->getDeviceInfo(id).outputChannels > 0) {
                devices += AudioDevice(audio, id);
            }
        }

        return devices;
        
    }

    bool AudioEngine::apiSupported(RtAudio::Api api) {
        
        std::vector<RtAudio::Api> apis;
        RtAudio::getCompiledApi(apis);

        for(auto const& elem : apis) {
            if (api == elem) { return true; }
        }
        
        return false;
        
    }

    void AudioEngine::opened() { }
    
    void AudioEngine::closed() { }
    
    size_t AudioEngine::findBestSampleRate() const {

        const auto rates = supportedSampleRates();

        nthrowif(rates.size() == 0, "Input and output devices have incompatible sample rates!");

        for (const auto& sampleRate : rates) {
            if (sampleRate >= 44100) { return sampleRate; }
        }

        return rates.last();
        
    }
}

