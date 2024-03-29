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

        Color fromColor = Color::white;
        Color toColor = Color::white;
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
            Color borderColor = Color::white;
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

        void draw(Graphics const& graphics) override {

            fader.update(graphics.deltaTime());

            graphics.setColor(color());

            const Transform transform = globalTransform();

            graphics.drawRoundedRect(transform, cornerRadius);

            graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);
            graphics.setColor(borderColor);
            graphics.setStrokeWidth(0.5);

            graphics.drawRoundedRect(transform, cornerRadius);

            for(const auto& child : children()) {
                child->draw(graphics);
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
        Color borderColor = Color::clear;
        ColorFader fader;
        Float32 cornerRadius = 0.0f;
        Function<void(Button*)> action = nullptr;

    };

}