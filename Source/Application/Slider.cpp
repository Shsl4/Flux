﻿#include <Application/Slider.h>

namespace Flux {
    
    Slider::Slider(const Point &p, Float32 length, Float32 width) : Component(p, { length, width + 15 }), sliderWidth(width) {

        setColor(scheme.darkest);
        
        this->text = Factory::createComponent<Text>(Point(0.0, width), Point(length, 15),
                                                    labelText, 8.0f,
                                                    VAlignment::center, HAlignment::center);

    }
    
    void Slider::drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        if(button == MouseButton::Left){

            Transform t = globalTransform();
            
            Range<Float32> rg = Range<Float32>(t.position.x, t.position.x + t.size.x);
            const auto v = f64(rg.translateTo(rg.clamp(f32(x)), Range<Float32>::linear));
            
            if(usesLogarithmicProgress()){

                const Range<Float64> logRange = { log10(range().min()), log10(range().max()) };
                const Float64 logValue = Range<Float64>::linear.translateTo(v, logRange);
                const Float64 value = range().clamp(std::pow(10.0f, logValue));
                setValue(value);

            }
            else{

                const Float64 value = Range<Float64>::linear.translateTo(v, range());
                setValue(value);

            }
            
            updateText();

            for(auto const& listener : listeners){
                listener->valueChanged(this, this->currentValue);
            }
            
        }

    }

    void Slider::doubleClick(MouseButton button, Float64 x, Float64 y) {

        if(button == MouseButton::Left) {
            setValue(defaultValue, true);
        }

    }

    void Slider::updateText() {

        if(formatFunction){
            this->text->setText(formatFunction(currentValue, labelExtension));
            return;
        }

        if(precision > 0){
            const String fmt ="{." + String::fromInteger(precision) + "}{}";
            this->text->setText(String::format(fmt, currentValue, labelExtension));

        }
        else{
            this->text->setText(String::format("{}{}", i32(currentValue), labelExtension));
        }

    }

    void Slider::initialize() {
        addChild(text);
    }

    void Slider::buttonUp(MouseButton button, Float64 x, Float64 y, Reactive *upTarget) {

        if(button == MouseButton::Left){
            text->setText(labelText);
        }

    }

    void Slider::draw(Graphics const& graphics) {

        graphics.setColor(scheme.darkest);
        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);
        graphics.setAntiAliasing(true);
        graphics.setStrokeWidth(sliderWidth);

        const Transform t = globalTransform();
        const Point p = t.position;

        graphics.drawLine(p, p + Point(size().x, 0.0f));

        graphics.setColor(scheme.lightest);

        const auto sizeRange = Range<Float64>(0.0, f64(size().x));
        const Float64 offset = valueRange.translateTo(currentValue, sizeRange);
        const Point end = p + Point(f32(offset), 0.0f);
            
        graphics.drawLine(p, end);

        graphics.setColor(Color::lighterGray);
        graphics.setStrokeCap(Graphics::StrokeCap::round);
        graphics.setStrokeWidth(f32(sliderWidth) / 2.0f);

        graphics.drawLine(end - Point(0.0f, sliderWidth / 2.0f), end + Point(0.0f, sliderWidth / 2.0f));
        
        graphics.resetPaint();
        
        text->draw(graphics);
        
    }

    void Slider::addListener(Slider::Listener *listener) {
        if(listener) this->listeners += listener;
    }

    void Slider::removeListener(Slider::Listener* listener) {
        this->listeners.removeAll(listener);
    }

    void Slider::setValue(Float64 newValue, bool notify) {

        if(Math::deq(currentValue, newValue)) return;

        this->currentValue = valueRange.clamp(newValue);

        refreshProgress();

        if(notify){

            for(auto const& listener : listeners){
                listener->valueChanged(this, this->currentValue);
            }

        }

    }

    void Slider::setRange(const Range<Float64> &range) {

        this->valueRange = range;

        if(this->currentValue < range.min()){
            setValue(range.min(), false);
        }

        if(this->currentValue > range.max()){
            setValue(range.max(), false);
        }

        this->defaultValue = valueRange.clamp(defaultValue);

        refreshProgress();

    }

    void Slider::setLabelText(const String &value) {
        this->labelText = value;
        this->text->setText(labelText);
    }

    void Slider::setLabelExtension(const String &value) {
        this->labelExtension = value;
    }

    void Slider::setLabelPrecision(UInt p) {
        this->precision = p;
    }

    void Slider::setDefaultValue(Float64 value) {
        this->defaultValue = valueRange.clamp(value);
        setValue(defaultValue);
        refreshProgress();
    }

    void Slider::buttonDown(MouseButton button, Float64 x, Float64 y) {

        if(button == MouseButton::Left){

            updateText();

        }

    }

    void Slider::setScheme(const ColorScheme& newScheme) {
        this->scheme = newScheme;
        setColor(scheme.darkest);

    }

    void Slider::useLogarithmicProgress(bool value) {
        this->logarithmicProgress = value;
        setValue(this->currentValue, false);
    }

    void Slider::refreshProgress() {

    }

    void Slider::useCustomFormatFunction(Function<String(Float64, const String &)> const& function) {
        this->formatFunction = function;
    }

}