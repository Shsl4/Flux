#include <Audio/AudioDevice.h>
#include <rtaudio/RtAudio.h>

namespace Flux {

    Int audioCallback(void* outBuffer, void* inBuffer, unsigned int bufferSize,
        double streamTime, RtAudioStreamStatus status, void* data) {

        static_cast<AudioDevice*>(data)->process(static_cast<Float64*>(outBuffer));
        return 0;
        
    }

    AudioDevice::AudioDevice() {
        
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
        
        this->outputDevice = audio->getDefaultOutputDevice();

        registerAudioCommands();
        
    }

    AudioDevice::~AudioDevice() {
        closeOutputDevice();
        Allocator<RtAudio>::destroy(audio);
    }

    void AudioDevice::listAudioDevices() const {

        const UInt count = audio->getDeviceCount();

        if(count == 0) {

            Console::log("There are no audio devices available.\n");
            return;
            
        }

        Console::log("Listing audio devices:\n");

        for (UInt index = 0; index < count; ++index) {

            RtAudio::DeviceInfo info = audio->getDeviceInfo(index);

            if(audio->isStreamOpen() && index == outputDevice) {
                Console::log("[{}] -> {} (In: {}, Out: {}) (Active)\n", index, info.name.c_str(), info.inputChannels, info.outputChannels);
            }
            else {
                Console::log("[{}] -> {} (In: {}, Out: {})\n", index, info.name.c_str(), info.inputChannels, info.outputChannels);
            }
            
        }
        
    }

    void AudioDevice::initialize(const Float64 rate, const UInt size) {

        assert(rate > 0.0);
        assert(size >= 16);
                
        this->sr = rate;
        this->bufSize = size;

        openMidiDevice(0);
        openOutputDevice(outputDevice);
        
    }

    void AudioDevice::closeOutputDevice() {

        if(audio->isStreamOpen()) {
            audio->closeStream();
            outputDeviceClosed();
        }
        
    }

    void AudioDevice::openOutputDevice(UInt deviceId) {

        closeOutputDevice();

        RtAudio::DeviceInfo info;
        
        try {
            
            info = audio->getDeviceInfo(deviceId);
            
        }
        catch(RtAudioError&){
            
            Console::log("Tried to open an invalid output device ({})\n", deviceId);
            return;
            
        }

        if (info.outputChannels == 0) {
            Console::log("The selected device is not an output device ({})\n", info.name.c_str());
            return;
        }
        
        RtAudio::StreamParameters parameters;
        parameters.deviceId = deviceId;
        parameters.nChannels = info.outputChannels;

        this->outputChannels = info.outputChannels;
        
        RtAudio::StreamOptions options;
        options.flags = RTAUDIO_NONINTERLEAVED;
        
        try {
            
            audio->openStream(&parameters, nullptr, RTAUDIO_FLOAT64, static_cast<UInt>(sr),
                              &bufSize, &audioCallback, this, &options);
            audio->startStream();
            
        }
        catch (RtAudioError& e) {
            
            Console::log("Failed to open audio device: {}\n", e.getMessage().c_str());
            return;
            
        }

        outputDeviceOpened();

        prepare(sr, bufSize);
        
        Console::log("Now using audio output device {} ({})\n", deviceId, info.name.c_str());
        
    }

    void AudioDevice::setSampleRate(Float64 value) {

        assert(value > 0.0);
        this->sr = value;
        openOutputDevice(outputDevice);
        
    }
    
    void AudioDevice::setBufferSize(UInt value) {

        assert(value >= 16);
        this->bufSize = value;
        openOutputDevice(outputDevice);

    }

    void AudioDevice::registerAudioCommands() {

        /*
        const auto openNode = CommandNode::make("Audio.Device.Open");
        const auto listNode = CommandNode::make("Audio.Device.List");
        const auto closeNode = CommandNode::make("Audio.Device.Close");

        openNode->setNodeDescription("Opens an audio output device.");
        listNode->setNodeDescription("Lists the available audio devices.");
        closeNode->setNodeDescription("Closes the current audio output device.");

        openNode->addArgument("id", ArgumentType::Int64)
                ->addExecutable([this](const CommandContext* context) {

                    UInt id = static_cast<UInt>(abs(context->getInt64("id")));

                    openOutputDevice(id);
                    
                });

        listNode->addExecutable([this](const auto*) {

            listAudioDevices();
            
        });

        closeNode->addExecutable([this](const auto*) {

            closeOutputDevice();
            
        });

        Console::registerCommand(openNode);
        Console::registerCommand(listNode);
        Console::registerCommand(closeNode);
        */
    }
    
    UInt AudioDevice::numOutputChannels() const { return this->outputChannels; }
    
    UInt AudioDevice::numInputChannels() const { return this->inputChannels; }
    
    Float64 AudioDevice::sampleRate() const { return this->sr; }
    
    UInt AudioDevice::bufferSize() const { return this->bufSize; }
    
    void AudioDevice::outputDeviceOpened() { }
    
    void AudioDevice::outputDeviceClosed() { }

}

