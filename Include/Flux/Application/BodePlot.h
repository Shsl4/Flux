#pragma once

#include <UI/Component.h>

#include <Audio/Effects/Filters/Filter.h>
#include <UI/Text.h>

namespace Flux {
    
    template<typename NumberType>
    struct Range{
        
        Range(NumberType min, NumberType max) : minValue(min), maxValue(max) {}

        NODISCARD static Range makeLinearRange() { return { static_cast<NumberType>(0), static_cast<NumberType>(1) }; }

        NODISCARD FORCEINLINE NumberType min() const{ return this->minValue; }

        NODISCARD FORCEINLINE NumberType max() const{ return this->maxValue; }

        NODISCARD FORCEINLINE NumberType clamp(NumberType v) const { return Math::clamp(v, minValue, maxValue); }

        NODISCARD FORCEINLINE NumberType clampAround(NumberType v) const { return Math::clampAround(v, minValue, maxValue); }

        static NumberType translateValue(NumberType value, Range fromRange, Range toRange){

            NumberType fromValue = fromRange.max() - fromRange.min();
            NumberType toValue = toRange.max() - toRange.min();
            return (value - fromRange.min()) * toValue / fromValue + toRange.min();

        }

    private:

        NumberType minValue;
        NumberType maxValue;

    };
        
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
        
        BodePlot(Point const& p, Point const& s);

        void doubleClick(MouseButton button, Float64 x, Float64 y) override;

        void drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void initialize() override;
        
        void setFilter(Audio::Filter* fil);

        void keyDown(Key const& key) override;

        void setCallback(Function<void(BodePlot*)> const& valueChanged);

    protected:
        
        void modified() override;
        
    private:

        enum class DrawMode {
            frequency,
            phase
        };
        
        void realignTexts() const;

        void drawGrid(SkCanvas* canvas, Float64 deltaTime);

        void recalculatePhaseResponse();

        void recalculateFrequencyResponse();

        void recalculatePath();

        Function<void(BodePlot*)> callback = nullptr;

        SkPath path;
        Audio::Filter* filter = nullptr;
        ColorScheme scheme = ColorScheme::coral;
        Map<Float32, Text*> frequencyTexts = {};
        MutableArray<Text*> gainTexts = {};
        Float32 textSize = 12.0f;

        DrawMode mode = DrawMode::frequency;

        static inline const MutableArray<Float32> gainsToDraw = { -18.0f, -12.0f, -6.0f, 0.0f, 6.0f, 12.0f, 18.0f };
        static inline const MutableArray<Float32> phasesToDraw = { -315.0f, -270.0f, -225.0f, -180.0f, -135.0f, -90.0f, -45.0f };

        static const inline Range<Float32> phaseRange = { -360.0f, 0.0f };
        static const inline Range<Float64> phaseRange64 = { -360.0, 0.0 };
        static const inline Range<Float32> gainRange = { -20.0f, 20.0f };

        
    };

}
