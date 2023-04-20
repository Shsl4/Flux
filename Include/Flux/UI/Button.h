#pragma once

#include <Flux/UI/Text.h>
#include <Flux/UI/Animation.h>

namespace Flux{

    class ColorFader : public Animation {

    public:

        explicit ColorFader(Component* comp) : target(comp){
            setDuration(0.10f);
        }

        void fadeTo(Color const& color){

            stop();

            fromColor = target->color();
            toColor = color;

            play();

        }

    protected:

        void process(Float64 deltaTime) override {
            target->setColor(Math::lerp(fromColor, toColor, f32(progress())));
        }

    private:

        Color fromColor = Colors::white;
        Color toColor = Colors::white;
        Component* target = nullptr;

    };

    class Button : public Component {

    public:

        struct Style {

            Style() = default;
            
            Style(const Float32 fontSize, const ColorScheme& scheme, const Color& borderColor, const HAlignment hAlign,
                const VAlignment vAlign)
                : fontSize(fontSize),
                  scheme(scheme),
                  borderColor(borderColor),
                  hAlign(hAlign),
                  vAlign(vAlign) {}

            Float32 fontSize = 12.0;
            ColorScheme scheme = ColorScheme::flatBlue;
            Color borderColor = Colors::white;
            HAlignment hAlign = HAlignment::center;
            VAlignment vAlign = VAlignment::center;
            
        };

        Button(Point const& pos, Point const& size, String const& label) : Component(pos, size), fader(this) {

            this->buttonLabel = Factory::createComponent<Text>(Point::zero, size, label, size.y / 2.0f, VAlignment::center, HAlignment::center);
            this->buttonLabel->setReactive(false);
            setColor(defaultColor);

        }

        NODISCARD FORCEINLINE Style style() const {

            return Style(label()->textSize(), ColorScheme::fromColor(defaultColor), borderColor,
                label()->hAlignment(), label()->vAlignment());
            
        }

        void draw(SkCanvas *canvas, Float64 deltaTime) override {

            fader.update(deltaTime);

            SkPaint paint;

            paint.setColor(color().skColor());

            const Point pos = globalTransform().position;
            const SkRect rect = SkRect::MakeXYWH(pos.x, pos.y, size().x, size().y);

            canvas->drawRoundRect(rect, cornerRadius, cornerRadius, paint);

            paint.setStyle(SkPaint::Style::kStroke_Style);
            paint.setColor(borderColor.skColor());
            paint.setStrokeWidth(0.5);

            canvas->drawRoundRect(rect, cornerRadius, cornerRadius, paint);

            for(const auto& child : children()) {
                child->draw(canvas, deltaTime);
            }

        }

        void setBorderColor(Color const& color) {
            this->borderColor = color;
        }
        
        void initialize() override {
            addChild(buttonLabel);
        }

        void buttonDown(MouseButton button, Float64 x, Float64 y) override {
            fader.fadeTo(pressedColor);
        }

        void buttonUp(MouseButton button, Float64 x, Float64 y, Reactive *upTarget) override {
            fader.fadeTo(upTarget == this ? hoveredColor : defaultColor);
            if (upTarget == this && action){
                action(this);
            }
        }

        void hovered() override {
            fader.fadeTo(hoveredColor);
        }

        void unHovered() override {
            if(!isPressed(MouseButton::Left)){
                fader.fadeTo(defaultColor);
            }
        }

        void setAction(Function<void(Button*)> const& value){
            this->action = value;
        }

        void setColorScheme(ColorScheme const& scheme){

            this->hoveredColor = scheme.darker;
            this->defaultColor = scheme.base;
            this->pressedColor = scheme.darkest;

            setColor(defaultColor);

        }

        void setCornerRadius(Float32 radius) { this->cornerRadius = radius; }

        NODISCARD FORCEINLINE Text* label() const { return this->buttonLabel; }

        void setStyle(Style const& style) {

            label()->setTextSize(style.fontSize);
            label()->setAlignment(style.vAlign, style.hAlign);
            setColorScheme(style.scheme);
            this->borderColor = style.borderColor;
            
        }
        
    protected:
        
        void modified() override {
            this->buttonLabel->setSize(size());
        }
        
    private:

        Text* buttonLabel;
        Color hoveredColor = ColorScheme::flatBlue.darker;
        Color defaultColor = ColorScheme::flatBlue.base;
        Color pressedColor = ColorScheme::flatBlue.darkest;
        Color borderColor = Colors::transparent;
        ColorFader fader;
        Float32 cornerRadius = 0.0f;
        Function<void(Button*)> action = nullptr;

    };

}