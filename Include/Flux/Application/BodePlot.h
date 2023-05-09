#pragma once

#include <UI/Component.h>

#include <Audio/Effects/Filters/Filter.h>
#include <Utility/Range.h>
#include <UI/Text.h>
#include <valarray>
#include <complex>
#include <KissFFT/kiss_fft.h>
#include "Utility/Timer.h"

struct Bin {
    Float64 gain;
    Float64 frequency;
};

struct CircularBuffer {

    explicit CircularBuffer(size_t size) : buffer(MutableArray<Float64>::filled(size)) {
        memset(buffer.data(), 0, size * sizeof(Float64));
    }

    FORCEINLINE void feed(const Float64* data, size_t size){

        if(!data) return;

        size_t offset = 0;
        size_t left = size;

        while(head + left > buffer.size()){
            const size_t diff = buffer.size() - head;
            memcpy(buffer.data() + head, data + offset, sizeof(Float64) * diff);
            offset += diff;
            left -= diff;
            head = 0;
        }

        memcpy(buffer.data() + head, data + offset, sizeof(Float64) * left);
        head = (left + head == buffer.size()) ? 0 : (head + left);

    }

    MutableArray<Float64> buffer = {};

private:

    size_t head = 0;

};

namespace Nucleus{

    template<>
    class Fmt<Bin>{

    public:

        static String format(Bin const& elem, String const& params) {
            return String::format("({.2}, {.1})\n", elem.gain, elem.frequency);
        }

    };

    template<>
    class Fmt<kiss_fft_cpx>{

    public:

        static String format(kiss_fft_cpx const& elem, String const& params) {
            return String::format("({.2}, {.2})\n",elem.r, elem.i);
        }

    };
}

namespace Flux {
        
    inline Float64 freqToRad(Float64 f, Float64 ny) {
        return (f * Math::pi<Float64>) / ny;
    }
    
    inline MutableArray<Float64> distributeAround(Float64 freq, Float64 ny, UInt count) {

        auto values = MutableArray<Float64>(count);
        const Float64 baseRad = freqToRad(9.0, ny);

        // todo: distribute around f
        for (size_t i = 0; i < count; ++i) {
            values += baseRad + (Math::pi<Float64> - baseRad) * pow(f64(i) / f64(count), 5.0);
        }

        return values;

    }

    class BodePlot : public Component {
        
    public:

        class Listener {

        public:

            virtual void valueChanged(BodePlot* plot, Float64 f, Float64 q) = 0;

            virtual ~Listener() = default;

        };
        
        BodePlot(Point const& p, Point const& s);

        void doubleClick(MouseButton button, Float64 x, Float64 y) override;

        void drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

        void draw(Graphics const& graphics) override;

        void initialize() override;
        
        void setFilter(Audio::Filter* fil);

        void keyDown(Key const& key) override;

        void recalculatePath();
        
        void addListener(Listener* listener);

        void removeListener(Listener* listener);

        void realignTexts() const;

        void setScheme(const ColorScheme& newScheme);

        void feedBuffer(Float64* block);

        NODISCARD FORCEINLINE Audio::Filter* fil() const { return this->filter; }
        static constexpr size_t spectrumWindowSize = 2048;

    protected:

        void willDispose() override;

        void modified() override;
        
    private:

        enum class DrawMode {
            frequency,
            phase
        };
        
        void drawGrid(Graphics const& graphics);

        void recalculatePhaseResponse();

        void recalculateFrequencyResponse();

        void recalculateSpectrum();

        void processFFT();

        Path path;
        Path spectrumPath;
        MutableArray<Listener*> listeners = {};
        Audio::Filter* filter = nullptr;
        DrawMode mode = DrawMode::frequency;
        ColorScheme scheme = ColorScheme::darkScheme(Color::tintGreen);
        Map<Float32, Text*> frequencyTexts = {};
        MutableArray<Text*> gainTexts = {};
        Float32 textSize = 12.0f;
        Timer timer;

        static inline const MutableArray<Float32> gainsToDraw = { -18.0f, -12.0f, -6.0f, 0.0f, 6.0f, 12.0f, 18.0f };
        static inline const MutableArray<Float32> phasesToDraw = { -315.0f, -270.0f, -225.0f, -180.0f, -135.0f, -90.0f, -45.0f };

        static const inline Range<Float32> phaseRange = { -360.0f, 0.0f };
        static const inline Range<Float64> phaseRange64 = { -360.0, 0.0 };
        static const inline Range<Float32> gainRange = { -20.0f, 20.0f };
        
        CircularBuffer circularBuffer = CircularBuffer(spectrumWindowSize);
        MutableArray<kiss_fft_cpx> cplx = {};
        MutableArray<kiss_fft_cpx> cplxOut = {};
        MutableArray<Bin> bins = {};

        MutableArray<Float64> lastGains = {};

        kiss_fft_cfg cfg = nullptr;

        std::mutex mutex;

        Path lastSpectrumPath;


    };

}
