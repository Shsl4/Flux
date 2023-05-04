#include <Application/WaveFile.h>

namespace Flux {

    WaveFile::WaveFile(MutableArray<MutableArray<Float64>> const& data, size_t sampleRate) {
    
        this->chunkId = "RIFF";
        this->format = "WAVE";
        this->subChunk1Id = "fmt ";
        this->subChunk1Size = 16;
        this->audioFormat = 1;
        this->channelCount = static_cast<UInt16>(data.size());
        this->sampleRate = static_cast<UInt32>(sampleRate);
        this->bitsPerSample = 32;
        this->byteRate = this->sampleRate * channelCount * bitsPerSample / 8;
        this->blockAlign = channelCount * bitsPerSample / 8;
        
        this->samplesPerChannel = data[0].size();
        
        this->subChunk2Id = "data";
        this->subChunk2Size = static_cast<UInt32>(samplesPerChannel) * channelCount * bitsPerSample / 8;
        
        this->buffers = data;
        
        this->chunkSize = 36 + subChunk2Size;

    }

    Float64 threeBytesToNormalizedFloat(MutableArray<unsigned char> const& data) {
    
        constexpr static Int32 int24Max = 8388607;
        
        if(data[2] & 0x80){
                        
            return f64((0xff << 24) | (data[2] << 16) | (data[1] << 8) | data[0]) / int24Max;
            
        }
        
        return f64((data[2] << 16) | (data[1] << 8) | data[0]) / int24Max;

    }


    template<typename Integral> requires std::is_integral_v<Integral>
    Float64 nextNormalized(File& file){
        return f64(file.read<Integral>()) / std::numeric_limits<Integral>::max();
    }

    WaveFile::WaveFile(const String &path) {

        File file = File();

        file.load(path);
        
        this->name = path;

        this->chunkId = String(file.readMultiple<char>(4).data(), 4);

        nthrowif(chunkId != "RIFF", "Not a riff file.");

        this->chunkSize = file.read<UInt32>();
        this->format = String(file.readMultiple<char>(4).data(), 4);

        nthrowif(format != "WAVE", "Not a wave file.");

        auto subchunk = String(file.readMultiple<char>(4).data(), 4);
        auto subChunkSize = file.read<UInt32>();
        
        while(subchunk != "fmt "){
            
            for(UInt32 i = 0; i < subChunkSize; ++i){
                file.read<char>();
            }
            
            subchunk = String(file.readMultiple<char>(4).data(), 4);
            subChunkSize = file.read<UInt32>();
            
        }

        this->subChunk1Id = subchunk;
        this->subChunk1Size = subChunkSize;

        nthrowif(this->subChunk1Size == 40, "Extensible wave format is unsupported.");
        
        this->audioFormat = file.read<UInt16>();
        this->channelCount = file.read<UInt16>();
        this->sampleRate = file.read<UInt32>();
        this->byteRate = file.read<UInt32>();
        this->blockAlign = file.read<UInt16>();
        this->bitsPerSample = file.read<UInt16>();

        if(subChunkSize == 18){
            this->cbSize = file.read<UInt16>();
        }

        subchunk = String(file.readMultiple<char>(4).data(), 4);
        subChunkSize = file.read<UInt32>();
        
        while(subchunk != "data"){
            
            for(UInt32 i = 0; i < subChunkSize; ++i){
                file.read<char>();
            }
            
            subchunk = String(file.readMultiple<char>(4).data(), 4);
            subChunkSize = file.read<UInt32>();
            
        }
        
        this->subChunk2Id = subchunk;
        this->subChunk2Size = subChunkSize;

        this->samplesPerChannel = subChunk2Size / (channelCount * (bitsPerSample / 8));

        this->buffers = MutableArray<MutableArray<Float64>>(channelCount);

        for(size_t i = 0; i < channelCount; ++i){
            buffers += MutableArray<Float64>::filled(samplesPerChannel);
        }

        if(audioFormat == 3) {

            for(size_t i = 0; i < samplesPerChannel; ++i){

                for(size_t channel = 0; channel < channelCount; ++channel) {
                    buffers[channel][i] = f64(file.read<Float32>());
                }

            }
            
            return;
            
        }

        static const Map<UInt16, Function<Float64(File&)>> procMap = {
                {8, &nextNormalized<Int8> },
                {16, &nextNormalized<Int16> },
                {24, [](File& file) { return threeBytesToNormalizedFloat(file.readMultiple<UInt8>(3)); }},
                {32, &nextNormalized<Int32> },
        };

        try{

            Function<Float64(File&)>& function = procMap[bitsPerSample];

            for(size_t i = 0; i < samplesPerChannel; ++i){

                for(size_t channel = 0; channel < channelCount; ++channel) {
                    buffers[channel][i] = function(file);
                }

            }

        }
        catch (Exceptions::Exception const&){

            throw Exceptions::Exception("Unsupported bits per sample");

        }

    }

    bool WaveFile::write(const String &path) {

        File file = File();

        if(!file.openForWriting(path)) return false;

        file.write(&chunkId[0], chunkId.size());
        file.write(chunkSize);
        file.write(&format[0], format.size());
        file.write(&subChunk1Id[0], subChunk1Id.size());
        file.write(subChunk1Size);
        file.write(audioFormat);
        file.write(channelCount);
        file.write(sampleRate);
        file.write(byteRate);
        file.write(blockAlign);
        file.write(bitsPerSample);
        file.write(&subChunk2Id[0], subChunk2Id.size());
        file.write(subChunk2Size);

        if(bitsPerSample == 8){
            
            for(size_t sample = 0; sample < samplesPerChannel; ++sample){

                for (int channel = 0; channel < channelCount; ++channel) {

                    file.write(static_cast<Int8>(buffers[channel][sample] * std::numeric_limits<Int8>::max()));

                }

            }
            
        }
        
        if(bitsPerSample == 16){
            
            for(size_t sample = 0; sample < samplesPerChannel; ++sample){

                for (int channel = 0; channel < channelCount; ++channel) {

                    file.write(static_cast<Int16>(buffers[channel][sample] * std::numeric_limits<Int16>::max()));

                }

            }
            
        }

        if(bitsPerSample == 24){

            for(size_t sample = 0; sample < samplesPerChannel; ++sample){

                for (int channel = 0; channel < channelCount; ++channel) {

                    constexpr static Int32 int24Max = 8388607;
                    auto v = i32(buffers[channel][sample] * int24Max);

                    file.write(reinterpret_cast<const char*>(&v), 3);

                }

            }

        }
        
        if(bitsPerSample == 32){
            
            for(size_t sample = 0; sample < samplesPerChannel; ++sample){

                for (int channel = 0; channel < channelCount; ++channel) {

                    file.write(static_cast<Int32>(buffers[channel][sample] * std::numeric_limits<Int32>::max()));

                }

            }
            
        }

        return true;

    }

}
