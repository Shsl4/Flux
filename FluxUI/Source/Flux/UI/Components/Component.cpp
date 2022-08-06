#include <Components/Component.h>

namespace Flux::UserInterface {

    void Component::initialize() {
        
    }

    void Component::draw(SkCanvas* canvas) {

        if(color.isTransparent()) { return; }
        
        SkPaint paint;
        
        paint.setColor(color.toSkColor());
        canvas->drawRect({ position.x(), position.y(), scale.x(), scale.y() }, paint);
        
    }

    void Component::setBounds(Float32 x, Float32 y, Float32 width, Float32 height) {
        this->position = { x, y };
        this->scale = { width, height };
    }

    void Component::setColor(LinearColor const& value) { this->color = value; }
    
    void Component::setPosition(SkVector const& value) { this->position = value; }
    
    void Component::setScale(SkVector const& value) { this->scale = value; }
    
    SkVector Component::getPosition() const { return this->position; }
    
    SkVector Component::getScale() const { return this->scale; }
    
    LinearColor Component::getColor() const { return this->color; }
    
}