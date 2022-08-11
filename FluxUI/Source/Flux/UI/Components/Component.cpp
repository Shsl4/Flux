#include <Components/Component.h>
#include <skia/core/SkDrawable.h>
#include <Components/Compound.h>

namespace Flux::UserInterface {

    Component::~Component() {
        Console::logRuntime("Destroyed {}", getClassName());
    }

    void Component::initialize() {

    }

    void Component::cleanup() {
        
        Console::logRuntime("Cleaning up {}", getClassName());

    }

    void Component::draw(SkCanvas* canvas, Float64 deltaTime) {

        if(color.isTransparent()) { return; }
        
        SkPaint paint;
        const SkVector pos = getAbsolutePosition();
        const SkRect rect = SkRect::MakeXYWH(pos.x(), pos.y(), scale.x(), scale.y());
        
        paint.setColor(color.toSkColor());
        canvas->drawRect(rect, paint);
        
    }

    void Component::setBounds(Float32 x, Float32 y, Float32 width, Float32 height) {
        this->position = { x, y };
        this->scale = { width, height };
    }

    void Component::setColor(LinearColor const& value) { this->color = value; }
    
    void Component::setPosition(SkVector const& value) { this->position = value; }
    
    void Component::setScale(SkVector const& value) { this->scale = value; }
    
    void Component::discard() { this->pendingDiscard = true; }
    
    SkVector Component::getPosition() const { return this->position; }
    
    SkVector Component::getScale() const { return this->scale; }
    
    LinearColor Component::getColor() const { return this->color; }
    
    bool Component::isInBounds(SkVector const& value) const {

        const SkVector pos = getAbsolutePosition();
        
        return value.fX >= pos.fX && value.fX <= pos.fX + scale.fX &&
            value.fY >= pos.fY && value.fY <= pos.fY + scale.fY;
            
    }

    SkVector Component::getAbsolutePosition() const {

        SkVector finalPos = position;
        const Compound* value = parent;

        while (value) {

            finalPos += value->position;
            value = value->parent;
            
        }

        return finalPos;

    }

    SkVector Component::getAbsoluteCenteredPosition() const {

        SkVector pos = getAbsolutePosition();

        pos.fX += scale.fX / 2.0f;
        pos.fY += scale.fY / 2.0f;

        return pos;
        
    }

    UInt Component::getDepth() const {

        UInt depth = 0;
        const Compound* value = parent;

        while (value) {
            value = value->parent;
            ++depth;            
        }

        return depth;        
        
    }

    Compound* Component::getParent() const {
        return this->parent;
    }

}
