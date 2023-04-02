#pragma once

#include <Audio/AudioBuffer.h>

namespace Flux {

    struct WaveFile{

        explicit WaveFile(String const& path);

        explicit WaveFile(MutableArray<MutableArray<Float64>> const& data, size_t sampleRate);

        bool write(String const& path);

        String name;
        String chunkId;
        UInt32 chunkSize;
        String format;
        String subChunk1Id;
        UInt32 subChunk1Size;
        UInt16 audioFormat;
        UInt16 channelCount;
        UInt32 sampleRate;
        UInt32 byteRate;
        UInt16 blockAlign;
        UInt16 bitsPerSample;
        UInt16 cbSize;
        String subChunk2Id;
        UInt32 subChunk2Size;
        size_t samplesPerChannel;

        MutableArray<MutableArray<Float64>> buffers;

    };

}
