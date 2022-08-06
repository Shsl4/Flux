#include "AudioDevice.h"

#include <rtaudio/RtAudio.h>

#include "Flux/Core/Tools/Console/Console.h"
#include "Flux/Core/Tools/Console/CommandContext.h"

namespace Flux {

    Int audioCallback(void* outBuffer, void* inBuffer, unsigned int bufferSize,
        double streamTime, RtAudioStreamStatus status, void* data) {

        const auto device = static_cast<AudioDevice*>(data);
        device->process(static_cast<Float64*>(outBuffer));
        return 0;
        
    }

    AudioDevice::AudioDevice() {
        
        this->audio = allocator.construct();
        fassertf(audio, "Failed to initialize audio system.");
        this->currentOutputDevice = audio->getDefaultOutputDevice();
        registerAudioCommands();
        
    }

    AudioDevice::~AudioDevice() {
        closeOutputDevice();
        allocator.destroy(audio);
    }

    void AudioDevice::listAudioDevices() const {

        const UInt count = audio->getDeviceCount();

        if(count == 0) {

            Console::logRuntime("There are no audio devices available.");
            return;
            
        }

        Console::logStatus("Listing audio devices:");

        for (UInt index = 0; index < count; ++index) {

            RtAudio::DeviceInfo info = audio->getDeviceInfo(index);

            if(audio->isStreamOpen() && index == currentOutputDevice) {
                Console::logStatus("[{}] -> {} (In: {}, Out: {}) (Active)", index, info.name, info.inputChannels, info.outputChannels);
            }
            else {
                Console::logStatus("[{}] -> {} (In: {}, Out: {})", index, info.name, info.inputChannels, info.outputChannels);
            }
            
        }
        
    }

    void AudioDevice::initialize(const Float64 rate, const UInt size) {

        fassert(rate > 0.0);
        fassert(size >= 16);
                
        this->sampleRate = rate;
        this->bufferSize = size;
        
        openMidiDevice(1);
        openAudioOutputDevice(currentOutputDevice);
        
    }

    void AudioDevice::closeOutputDevice() {

        if(audio->isStreamOpen()) {
            audio->closeStream();
            onOutputDeviceClosed();
        }
        
    }

    void AudioDevice::openAudioOutputDevice(UInt deviceId) {

        closeOutputDevice();

        RtAudio::DeviceInfo info;
        
        try {
            
            info = audio->getDeviceInfo(deviceId);
            
        }
        catch(RtAudioError&){
            
            Console::logRuntime("Tried to open an invalid output device ({})", deviceId);
            return;
            
        }

        if (info.outputChannels == 0) {
            Console::logRuntime("The selected device is not an output device ({})", info.name);
            return;
        }
        
        RtAudio::StreamParameters parameters;
        parameters.deviceId = deviceId;
        parameters.nChannels = info.outputChannels;

        this->outputChannelCount = info.outputChannels;
        
        RtAudio::StreamOptions options;
        options.flags = RTAUDIO_NONINTERLEAVED;
        
        try {
            
            audio->openStream(&parameters, nullptr, RTAUDIO_FLOAT64, static_cast<UInt>(sampleRate),
                &bufferSize, &audioCallback, this, &options);
            audio->startStream();
            
        }
        catch (RtAudioError& e) {
            
            Console::logRuntime("Failed to open audio device: {}", e.getMessage());
            return;
            
        }

        onOutputDeviceOpened();

        prepare(sampleRate, bufferSize);
        
        Console::logStatus("Now using audio output device {} ({})", deviceId, info.name);
        
    }

    void AudioDevice::setSampleRate(Float64 value) {

        fassert(value > 0.0);
        this->sampleRate = value;
        openAudioOutputDevice(currentOutputDevice);
        
    }
    
    void AudioDevice::setBufferSize(UInt value) {

        fassert(value >= 16);
        this->bufferSize = value;
        openAudioOutputDevice(currentOutputDevice);

    }

    void AudioDevice::registerAudioCommands() {

        const auto openNode = CommandNode::make("Audio.Device.Open");
        const auto listNode = CommandNode::make("Audio.Device.List");
        const auto closeNode = CommandNode::make("Audio.Device.Close");


        openNode->setNodeDescription("Opens an audio output device.");
        listNode->setNodeDescription("Lists the available audio devices.");
        closeNode->setNodeDescription("Closes the current audio output device.");

        openNode->addArgument("id", ArgumentType::Int64)
                ->addExecutable([this](const CommandContext* context) {

                    UInt id = static_cast<UInt>(abs(context->getInt64("id")));

                    openAudioOutputDevice(id);
                    
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
        
    }
    
    UInt AudioDevice::getOutputChannelCount() const { return this->outputChannelCount; }
    
    UInt AudioDevice::getInputChannelCount() const { return this->inputChannelCount; }
    
    Float64 AudioDevice::getSampleRate() const { return this->sampleRate; }
    
    UInt AudioDevice::getBufferSize() const { return this->bufferSize; }
    
    void AudioDevice::onOutputDeviceOpened() { }
    
    void AudioDevice::onOutputDeviceClosed() { }

}

