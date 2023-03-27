#pragma once

#include <Audio/AudioBuffer.h>

namespace Flux {

    struct WaveFile{

    public:

        explicit WaveFile(String const& path);

        bool write(String const& path);

        String chunkId;
        Int32 chunkSize;
        String format;
        String subChunk1Id;
        Int32 subChunk1Size;
        Int16 audioFormat;
        Int16 channelCount;
        Int32 sampleRate;
        Int32 byteRate;
        Int16 blockAlign;
        Int16 bitsPerSample;
        String subChunk2Id;
        Int32 subChunk2Size;
        size_t samplesPerChannel;

        MutableArray<MutableArray<Float64>> buffers;

    };

}