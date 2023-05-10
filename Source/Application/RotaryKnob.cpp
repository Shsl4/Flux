#include <Application/RotaryKnob.h>

namespace Flux {


    RotaryKnob::RotaryKnob(const Point &p, Float32 radius) : Slider(p, radius, radius) {

    }

    void RotaryKnob::drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        if(button == MouseButton::Left){

            rotation = Math::clamp(rotation + deltaY * 0.35, 0.0, 300.0);

            if(usesLogarithmicProgress()){

                const Range<Float64> logRange = { log10(range().min()), log10(range().max()) };
                const Float64 logValue = Range<Float64>::linear.translateTo(rotation / 300.0, logRange);
                const Float64 value = range().clamp(std::pow(10.0f, logValue));
                setValue(value);

            }
            else{

                const Float64 value = Range<Float64>::linear.translateTo(rotation / 300.0, range());
                setValue(value);

            }

            updateText();
            
        }

    }

    void RotaryKnob::draw(Graphics const& graphics) {

        const Transform t = globalTransform();
        const Point& pos = t.position;
        const Point& s = t.size;
        const Float32 rad = s.x / 2.0f;
        const Float32 diff = s.x * 0.125f;

        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);
        graphics.setStrokeWidth(size().x / 50.0f);
        graphics.setAntiAliasing(true);

        const Point arcPos = {pos.x + diff, pos.y + diff};
        const Point arcSize = {s.x - 2.0f * diff, s.x - 2.0f * diff};

        graphics.setColor(colorScheme().base);
        graphics.drawArc(arcPos, arcSize, 120, 300, false);

        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);
        graphics.setStrokeWidth(size().x / 50.0f);
        graphics.setAntiAliasing(true);

        graphics.setColor(colorScheme().lightest);
        graphics.drawArc(arcPos, arcSize, 120, f32(rotation), false);

        graphics.setColor(color());
        graphics.drawCircle({ pos.x + s.x / 2.0f, pos.y + s.x / 2.0f }, rad * 0.6f);

        for (auto const& child : children()) {
            child->draw(graphics);
        }

    }

    
    void RotaryKnob::refreshProgress() {

        Range<Float64> fRange = { 0.0, 300.0 };

        if(usesLogarithmicProgress()){

            const Range<Float64> logRange = { log10(range().min()), log10(range().max()) };
            this->rotation = logRange.translateTo(log10(this->value()), fRange);

        }
        else{
            this->rotation = range().translateTo(value(), fRange);
        }

    }

}