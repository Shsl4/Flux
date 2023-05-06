#include <Application/Slider.h>

namespace Flux {
    
    Slider::Slider(const Point &p, const Point &s) : Component(p, s) {

        setColor(scheme.darkest);
        Point sz = size();
        this->text = Factory::createComponent<Text>(Point::zero, sz, labelText, Math::min(sz.x, sz.y) / 3.0f, VAlignment::bottom, HAlignment::center);
        text->setReactive(false);

    }
    
    void Slider::drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        if(button == MouseButton::Left){

            Transform t = globalTransform();
            
            Range<Float32> rg = Range<Float32>(t.position.x, t.position.x + t.size.x);
            auto v= Range<Float32>::translateValue(rg.clamp(f32(x)), rg, Range<Float32>::makeLinearRange());
            
            if(usesLogarithmicProgress()){

                const Range<Float64> logRange = { log10(range().min()), log10(range().max()) };
                const Float64 logValue = Range<Float64>::translateValue(v, Range<Float64>::makeLinearRange(), logRange);
                const Float64 value = range().clamp(std::pow(10.0f, logValue));
                setValue(value);

            }
            else{

                const Float64 value = Range<Float64>::translateValue(v,
                                                                    Range<Float64>::makeLinearRange(),
                                                                    range());
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

        Component::draw(graphics);
        
        graphics.setColor(scheme.darkest);
        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);
        graphics.setAntiAliasing(true);
        graphics.setStrokeWidth(size().y);

        const Transform t = globalTransform();
        const Point p = t.position + Point(0.0f,  t.size.y / 2.0f);

        graphics.drawLine(p, p + Point(size().x, 0.0));

        graphics.setColor(scheme.lightest);

        const auto sizeRange = Range<Float64>(0.0, f64(size().x));
        const Float64 offset = Range<Float64>::translateValue(currentValue, valueRange, sizeRange);
        const Point end = p + Point(f32(offset), 0.0);
            
        graphics.drawLine(p, end);

        graphics.setColor(Colors::lighterGray);
        graphics.setStrokeCap(Graphics::StrokeCap::round);
        graphics.setStrokeWidth(size().y / 4.0);

        graphics.drawLine(end - Point(0.0f, t.size.y / 2.0f), end + Point(0.0, t.size.y / 2.0f));
        
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

}