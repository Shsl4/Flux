#pragma once

#include <UI/Text.h>
#include <Utility/Range.h>

namespace Flux {

    class Slider : public Component {

    public:

        class Listener {

        public:

            virtual void valueChanged(Slider* knob, Float64 newValue) = 0;

            virtual ~Listener() = default;

        };

        Slider(Point const& p, Point const& s);

        void initialize() override;

        void drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

        void updateText();

        void doubleClick(MouseButton button, Float64 x, Float64 y) override;

        void buttonUp(MouseButton button, Float64 x, Float64 y, Reactive *upTarget) override;

        void draw(Graphics const& graphics) override;

        void addListener(Listener* listener);

        void removeListener(Listener* listener);

        void setValue(Float64 newValue, bool notify = true);

        void setLabelText(String const& value);

        void setLabelExtension(String const& value);

        void setDefaultValue(Float64 value);

        void setLabelPrecision(UInt p);

        void setRange(Range<Float64> const& range);

        void useLogarithmicProgress(bool value);

        void buttonDown(MouseButton button, Float64 x, Float64 y) override;

        void setScheme(const ColorScheme& newScheme);

        NODISCARD FORCEINLINE ColorScheme colorScheme() const { return this->scheme; }

        NODISCARD FORCEINLINE bool usesLogarithmicProgress() const { return this->logarithmicProgress; }

        NODISCARD FORCEINLINE Range<Float64> range() const { return this->valueRange; }

        NODISCARD FORCEINLINE Float64 value() const { return this->currentValue; }
    
    protected:

        virtual void refreshProgress();

        NODISCARD FORCEINLINE Text* label() const { return this->text; }
        
    private:

        ColorScheme scheme = ColorScheme::coolScheme(Colors::tintPurple);
        Float64 currentValue = 0.0;
        Float64 defaultValue = 0.5;
        Range<Float64> valueRange = { 0.0, 1.0 };
        Text* text = nullptr;
        String labelText = "Label";
        String labelExtension = "";
        UInt precision = 2;
        MutableArray<Listener*> listeners = {};
        
        bool logarithmicProgress = false;
        
    };

    
}