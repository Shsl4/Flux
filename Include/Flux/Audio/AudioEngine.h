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

        NODISCARD String name() const {

            if(!valid()) return "None";
            
            // Fixes a bug on macOS where if your device name contains an apostrophe, rtaudio would read it as \xd5
            // and produce a string that cannot be drawn by skia.
            return String(info().name).replaceOccurrences("\xd5", "'");

        }

        NODISCARD UInt32 inChannels() const { return valid() ? info().inputChannels : 0; }
        
        NODISCARD UInt32 outChannels() const { return valid() ? info().outputChannels : 0; }

        NODISCARD bool isInput() const { return inChannels() > 0; }
        
        NODISCARD bool isOutput() const { return outChannels() > 0; }

        NODISCARD Float64 sampleRate() const { return valid() ? info().currentSampleRate : 0; }
        
        NODISCARD Float64 bestSampleRate() const { return valid() ? info().preferredSampleRate : 0; }

        NODISCARD UInt32 deviceId() const { return id; }

        NODISCARD RtAudio::StreamParameters inParams() const {
            
            RtAudio::StreamParameters params = {};
            
            if(valid()){
                params.firstChannel = 0;
                params.deviceId = deviceId();
                params.nChannels = inChannels();
            }
            
            return params;
            
        }

        NODISCARD RtAudio::StreamParameters outParams() const {

            RtAudio::StreamParameters params = {};
            
            if(valid()){
                params.firstChannel = 0;
                params.deviceId = deviceId();
                params.nChannels = outChannels();
            }
            
            return params;
            
        }
                
        NODISCARD MutableArray<UInt> sampleRates() const {

            if(!valid()) return {};

            return info().sampleRates;
            
        }

        NODISCARD FORCEINLINE bool valid() const { return audio && id > 0; }
    
    private:

        NODISCARD RtAudio::DeviceInfo info() const { return audio->getDeviceInfo(id); }
        
        RtAudio* audio = nullptr;
        UInt32 id = 0;
        
    };

    class AudioEngine {

    public:

        class Listener {

        public:
            
            virtual void audioDeviceOpened(AudioDevice const& in, AudioDevice const& out) = 0;
            
            virtual void sampleRateChanged(size_t sampleRate) = 0;

            virtual ~Listener() = default;
            
        };

        AudioEngine();

        virtual ~AudioEngine();

        void initialize();

        void setSampleRate(UInt value);

        void setBufferSize(UInt value);

        void setInputDevice(AudioDevice const& dev);

        void setOutputDevice(AudioDevice const& dev);

        void setApi(RtAudio::Api value);

        void writeConfig();

        void close();
        
        void addListener(Listener* listener);

        void removeListener(Listener* listener);
        
        NODISCARD MutableArray<UInt> supportedSampleRates() const;

        NODISCARD static MutableArray<UInt> supportedBufferSizes();

        NODISCARD RtAudio::Api api() const;

        NODISCARD UInt numOutputChannels() const;

        NODISCARD UInt numInputChannels() const;

        NODISCARD UInt sampleRate() const;

        NODISCARD UInt bufferSize() const;
        
        NODISCARD AudioDevice inputDevice() const { return this->inDevice; }

        NODISCARD AudioDevice outputDevice() const { return this->outDevice; }

        NODISCARD MutableArray<AudioDevice> availableInputDevices() const;

        NODISCARD MutableArray<AudioDevice> availableOutputDevices() const;

        NODISCARD static bool apiSupported(RtAudio::Api api);
        
        virtual void prepare(Float64 rate, UInt size) = 0;

        virtual void process(Float64* inputBuffer, Float64* outputBuffer) = 0;
        
    protected:

        virtual void opened();

        virtual void closed();

    private:

        NODISCARD size_t findBestSampleRate() const;
        
        bool open();
        
        UInt bufSize = 0;
        UInt sr = 0;
        
        RtAudio* audio = nullptr;

        AudioConfig config;
        
        AudioDevice inDevice = {};
        AudioDevice outDevice = {};
        
        MutableArray<Listener*> listeners;

    };
    
}

namespace Nucleus {
    
    template<>
    class Serializer<Flux::AudioConfig> {

    public:
        
        static void serialize(Container* container, Flux::AudioConfig const& object) {

            container->add("api", RtAudio::getApiDisplayName(object.favoriteApi));
            container->add("input", object.favoriteInput);
            container->add("output", object.favoriteOutput);
            container->add("sampleRate", object.favoriteSampleRate);
            container->add("bufferSize", object.favoriteBufferSize);
        
        }
        
        static Flux::AudioConfig deserialize(const Container* container) {

            Flux::AudioConfig obj;

            obj.favoriteApi = RtAudio::getCompiledApiByDisplayName(container->get<String>("api").begin().get());
            obj.favoriteInput = container->get<String>("input");
            obj.favoriteOutput = container->get<String>("output");
            obj.favoriteSampleRate = container->get<size_t>("sampleRate");
            obj.favoriteBufferSize = container->get<size_t>("bufferSize");
            
            return obj;
        
        }
        
    };
    
}


