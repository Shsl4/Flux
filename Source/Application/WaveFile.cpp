#include <Application/WaveFile.h>

namespace Flux {

    WaveFile::WaveFile(const String &path) {

        File file = File();

        file.load(path);

        this->chunkId = file.readMultiple<char>(4).data();

        nthrowif(chunkId != "RIFF", "Not a riff file.");

        this->chunkSize = file.read<Int32>();
        this->format = file.readMultiple<char>(4).data();

        nthrowif(format != "WAVE", "Not a wave file.");

        this->subChunk1Id = file.readMultiple<char>(4).data();

        nthrowif(subChunk1Id != "fmt ", "Format chunk expected but not found.");

        this->subChunk1Size = file.read<Int32>();
        this->audioFormat = file.read<Int16>();
        this->channelCount = file.read<Int16>();
        this->sampleRate = file.read<Int32>();
        this->byteRate = file.read<Int32>();
        this->blockAlign = file.read<Int16>();
        this->bitsPerSample = file.read<Int16>();

        nthrowif(bitsPerSample != 16, "Only 16 bits wave files are supported.");

        this->subChunk2Id = file.readMultiple<char>(4).data();

        nthrowif(subChunk2Id != "data", "Data chunk expected but not found.");

        this->subChunk2Size = file.read<Int32>();

        this->samplesPerChannel = subChunk2Size / (channelCount * (bitsPerSample / 8));

        this->buffers = MutableArray<MutableArray<Float64>>(channelCount);

        for(size_t i = 0; i < channelCount; ++i){
            buffers += MutableArray<Float64>::filled(samplesPerChannel);
        }

        for(size_t i = 0; i < samplesPerChannel; ++i){

            for(size_t channel = 0; channel < channelCount; ++channel) {
                buffers[channel][i] = f64(file.read<Int16>()) / std::numeric_limits<Int16>::max();
            }

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

        for(size_t sample = 0; sample < samplesPerChannel; ++sample){

            for (int channel = 0; channel < channelCount; ++channel) {

                file.write(static_cast<Int16>(buffers[channel][sample] * std::numeric_limits<Int16>::max()));

            }

        }

        return true;

    }
}
