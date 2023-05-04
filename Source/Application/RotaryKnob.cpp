#include <Application/RotaryKnob.h>

namespace Flux {

    void RotaryKnob::drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        if(button == MouseButton::Left){

            rotation = Math::clamp(rotation + deltaY * 0.35, 0.0, 300.0);

            if(logarithmicProgress){

                const Range<Float64> logRange = { log10(valueRange.min()), log10(valueRange.max()) };
                const Float64 logValue = Range<Float64>::translateValue(rotation / 300.0, Range<Float64>::makeLinearRange(), logRange);
                const Float64 freq = valueRange.clamp(std::pow(10.0f, logValue));
                this->currentValue = freq;

            }
            else{

                this->currentValue = Range<Float64>::translateValue(rotation / 300.0,
                                                                    Range<Float64>::makeLinearRange(),
                                                                    valueRange);

            }

            updateText();

            for(auto const& listener : listeners){
                listener->valueChanged(this, this->currentValue);
            }
            
        }

    }

    void RotaryKnob::doubleClick(MouseButton button, Float64 x, Float64 y) {

        if(button == MouseButton::Left) {
            setValue(defaultValue, true);
        }

    }

    void RotaryKnob::updateText() {
        const String fmt = "{0," + String::fromInteger(precision) + "}{1}";
        this->text->setText(String::format(fmt, currentValue, labelExtension));
    }

    RotaryKnob::RotaryKnob(const Point &p, Float32 radius) : Component(p, { radius, radius * 1.25f }) {

        setColor(scheme.darkest);
        Point sz = size();
        this->text = Factory::createComponent<Text>(Point::zero, sz, labelText,
                                                    sz.x / 6.0f, VAlignment::bottom, HAlignment::center);

    }

    void RotaryKnob::initialize() {
        addChild(text);
    }

    void RotaryKnob::buttonUp(MouseButton button, Float64 x, Float64 y, Reactive *upTarget) {

        if(button == MouseButton::Left){
            text->setText(labelText);
        }

    }

    void RotaryKnob::draw(Graphics const& graphics) {

        const Transform t = globalTransform();
        const Point& pos = t.position;
        const Point& s = t.size;
        const Float32 rad = s.x / 2.0f;
        const Float32 diff = s.x * 0.125f;

        graphics.setStyle(Graphics::Style::Fill);
        graphics.setStrokeWidth(size().x / 10.0f);
        graphics.setAntiAliasing(true);

        const Point arcPos = {pos.x + diff, pos.y + diff};
        const Point arcSize = {s.x - 2.0f * diff, s.x - 2.0f * diff};

        graphics.setColor(scheme.base);
        graphics.drawArc(arcPos, arcSize, 120, 300, true);

        graphics.setColor(scheme.lightest);
        graphics.drawArc(arcPos, arcSize, 120, f32(rotation), true);

        graphics.setColor(color());
        graphics.drawCircle({ pos.x + s.x / 2.0f, pos.y + s.x / 2.0f }, rad * 0.6f);

        text->draw(graphics);
        text->setReactive(false);

    }

    void RotaryKnob::addListener(RotaryKnob::Listener *listener) {
        if(listener) this->listeners += listener;
    }

    void RotaryKnob::removeListener(RotaryKnob::Listener* listener) {
        this->listeners.removeAll(listener);
    }

    void RotaryKnob::setValue(Float64 newValue, bool notify) {

        if(Math::deq(currentValue, newValue)) return;

        this->currentValue = valueRange.clamp(newValue);

        Range<Float64> fRange = { 0.0, 300.0 };

        if(logarithmicProgress){

            const Range<Float64> logRange = { log10(valueRange.min()), log10(valueRange.max()) };
            this->rotation = Range<Float64>::translateValue(log10(this->currentValue), logRange, fRange);

        }
        else{
            this->rotation = Range<Float64>::translateValue(currentValue, valueRange, fRange);
        }

        if(notify){

            for(auto const& listener : listeners){
                listener->valueChanged(this, this->currentValue);
            }

        }

    }

    void RotaryKnob::setRange(const Range<Float64> &range) {

        this->valueRange = range;

        if(this->currentValue < range.min()){
            setValue(range.min(), false);
        }

        if(this->currentValue > range.max()){
            setValue(range.max(), false);
        }

        this->defaultValue = valueRange.clamp(defaultValue);

    }

    void RotaryKnob::setLabelText(const String &value) {
        this->labelText = value;
        this->text->setText(labelText);
    }

    void RotaryKnob::setLabelExtension(const String &value) {
        this->labelExtension = value;
    }

    void RotaryKnob::setLabelPrecision(UInt p) {
        this->precision = p;

    }

    void RotaryKnob::setDefaultValue(Float64 value) {
        this->defaultValue = valueRange.clamp(value);
        setValue(this->defaultValue);
    }

    void RotaryKnob::buttonDown(MouseButton button, Float64 x, Float64 y) {

        if(button == MouseButton::Left){

            updateText();

        }

    }

    void RotaryKnob::useLogarithmicProgress(bool value) {
        this->logarithmicProgress = value;
        setValue(this->currentValue, false);
    }

}