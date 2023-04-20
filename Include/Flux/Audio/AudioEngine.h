#pragma once

#include <MIDI/MidiManager.h>
#include <rtaudio/RtAudio.h>

class RtAudio;

namespace Flux {

    struct AudioConfig {
        
        RtAudio::Api favoriteApi = RtAudio::UNSPECIFIED;
        String favoriteInput = "";
        String favoriteOutput = "";
        size_t favoriteSampleRate = 0;
        size_t favoriteBufferSize = 0;
        
    };

    struct AudioDevice {

        AudioDevice() = default;
        
        AudioDevice(RtAudio* audio, const UInt32 id) : audio(audio), id(id) { }

        bool operator==(const AudioDevice& other) const {
            return valid() && other.valid() && other.id == this->id;
        }

        NODISCARD String name() const { return valid() ? info().name : "None"; }

        NODISCARD UInt32 inChannels() const { return valid() ? info().inputChannels : 0; }
        
        NODISCARD UInt32 outChannels() const { return valid() ? info().outputChannels : 0; }

        NODISCARD bool isInput() const { return inChannels() > 0; }
        
        NODISCARD bool isOutput() const { return outChannels() > 0; }

        NODISCARD Float64 sampleRate() const { return valid() ? info().currentSampleRate : 0; }
        
        NODISCARD Float64 bestSampleRate() const { return valid() ? info().preferredSampleRate : 0; }

        NODISCARD UInt32 deviceId() const { return id; }

        NODISCARD RtAudio::StreamParameters inParams() const {
            return valid() ? RtAudio::StreamParameters(deviceId(), inChannels(), 0) : RtAudio::StreamParameters();
        }

        NODISCARD RtAudio::StreamParameters outParams() const {
            return valid() ? RtAudio::StreamParameters(deviceId(), outChannels(), 0) : RtAudio::StreamParameters();
        }
                
        NODISCARD MutableArray<Float64> sampleRates() const {

            if(!valid()) return {};
            
            auto const v = info().sampleRates;
            MutableArray<Float64> rates(v.size());

            for(auto const& sr : v) {
                rates += f64(sr);
            }

            return rates;
            
        }

        NODISCARD FORCEINLINE bool valid() const { return audio && id > 0; }
    
    private:

        NODISCARD RtAudio::DeviceInfo info() const { return audio->getDeviceInfo(id); }
        
        RtAudio* audio = nullptr;
        UInt32 id = 0;
        
    };
    
    class AudioEngine {
        
    public:

        AudioEngine();
        
        virtual ~AudioEngine();

        void listDevices() const;
        
        void initialize(Float64 rate, UInt size);

        void setSampleRate(Float64 value);
        
        void setBufferSize(UInt value);

        void setInputDevice(AudioDevice const& dev);

        void setOutputDevice(AudioDevice const& dev);

        void writeConfig();
        
        void close();
        
        virtual void prepare(Float64 rate, UInt size) = 0;
        
        virtual void process(Float64* inputBuffer, Float64* outputBuffer) = 0;

        NODISCARD MutableArray<Float64> supportedSampleRates() const;

        NODISCARD MutableArray<size_t> supportedBufferSizes() const;

        NODISCARD RtAudio::Api api() const;
        
        NODISCARD UInt numOutputChannels() const;
        
        NODISCARD UInt numInputChannels() const;

        NODISCARD Float64 sampleRate() const;

        NODISCARD UInt bufferSize() const;

        NODISCARD AudioDevice inputDevice() const { return this->inDevice; }

        NODISCARD AudioDevice outputDevice() const { return this->outDevice; }
        
        NODISCARD MutableArray<AudioDevice> enumerateInputDevices() const;
        
        NODISCARD MutableArray<AudioDevice> enumerateOutputDevices() const;

        NODISCARD static bool apiSupported(RtAudio::Api api);
        
        void setApi(RtAudio::Api value);

        void addOpenCallback(Function<void()> const& callback);
    
    protected:

        virtual void opened();
        
        virtual void closed();
        
    private:

        RtAudio* audio = nullptr;

        bool open();

        AudioConfig config;
        AudioDevice inDevice = {};
        AudioDevice outDevice = {};
        
        UInt bufSize = 0;
        Float64 sr = 0.0;

        MutableArray<Function<void()>> openCallbacks;
        
    };
    
}


namespace Nucleus {

    inline String apiName(RtAudio::Api api) {
        
        switch (api) {
            case RtAudio::UNSPECIFIED: return "Invalid";
            case RtAudio::MACOSX_CORE: return "CoreAudio";
            case RtAudio::LINUX_ALSA: return "ALSA";
            case RtAudio::UNIX_JACK: return "JACK";
            case RtAudio::LINUX_PULSE: return "PulseAudio";
            case RtAudio::LINUX_OSS: return "Open Sound System";
            case RtAudio::WINDOWS_ASIO: return "ASIO";
            case RtAudio::WINDOWS_WASAPI: return "Windows WASAPI";
            case RtAudio::WINDOWS_DS: return "DirectSound";
            case RtAudio::RTAUDIO_DUMMY: return "Dummy";
            case RtAudio::NUM_APIS: return "Invalid";
        }

        return "Invalid";
        
    }

    inline RtAudio::Api nameToApi(String const& name) {

        if(name == "CoreAudio") return RtAudio::MACOSX_CORE;
        if(name == "ALSA") return RtAudio::LINUX_ALSA;
        if(name == "JACK") return RtAudio::UNIX_JACK;
        if(name == "PulseAudio") return RtAudio::LINUX_PULSE;
        if(name == "Open Sound System") return RtAudio::LINUX_OSS;
        if(name == "ASIO") return RtAudio::WINDOWS_ASIO;
        if(name == "Windows WASAPI") return RtAudio::WINDOWS_WASAPI;
        if(name == "DirectSound") return RtAudio::WINDOWS_DS;
        if(name == "Dummy") return RtAudio::RTAUDIO_DUMMY;

        return RtAudio::UNSPECIFIED;
        
    }
    
    template<>
    class Serializer<Flux::AudioConfig> {

    public:
        
        static void serialize(Container* container, Flux::AudioConfig const& object) {

            container->add("api", apiName(object.favoriteApi));
            container->add("input", object.favoriteInput);
            container->add("output", object.favoriteOutput);
            container->add("sampleRate", object.favoriteSampleRate);
            container->add("bufferSize", object.favoriteBufferSize);
        
        }
        
        static Flux::AudioConfig deserialize(const Container* container) {

            Flux::AudioConfig obj;

            obj.favoriteApi = nameToApi(container->get<String>("api"));
            obj.favoriteInput = container->get<String>("input");
            obj.favoriteOutput = container->get<String>("output");
            obj.favoriteSampleRate = container->get<size_t>("sampleRate");
            obj.favoriteBufferSize = container->get<size_t>("bufferSize");
            
            return obj;
        
        }
        
        
    };
    
}


