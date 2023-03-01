#include "Component.h"

namespace Flux::UI {
    
    Component::Component(Transform const& t): localTransform(t) {
            
    }

    Component::Component(Point const& p, Point const& s) : localTransform({ p, s, 0.0f }) {
        
    }

    void Component::draw(SkCanvas* canvas, Float64 deltaTime) {

        if(renderColor.transparent()) { return; }
        
        SkPaint paint;
        const auto t = globalTransform();
        const SkRect rect = SkRect::MakeXYWH(t.position.x, t.position.y, t.size.x, t.size.y);
        
        paint.setColor(renderColor.skColor());
        canvas->drawRect(rect, paint);

        for(const auto& child : childrenArray) {
            child->draw(canvas, deltaTime);
        }
        
    }
    
    Component* Component::addChild(Component* component) {

        if(!Factory::valid(this)) {
            throwException("Trying to add child on an invalid component! Use Component::Factory::create() to create components.");
        }

        if(!Factory::valid(component)) {
            throwException("Trying to add an invalid component! Use Component::Factory::create() to create components.");
        }

        if (component->parentComponent) {
            throwException("Trying to add a component that already has a parent!");
        }

        if (component == this) {
            throwException("Trying to add a component to itself!");
        }

        component->parentComponent = this;
        childrenArray += component;
        childAdded(component);
        component->parentLinked();

        return component;
        
    }
    
    void Component::dispose() {

        willDispose();

        for (size_t i = childrenArray.size(); i > 0; --i) {
            childrenArray[i - 1]->dispose();
        }
        
        Factory::dispose(this);

        parentComponent = nullptr;
        
    }

    void Component::setPosition(Point const& p) {
        this->localTransform.position = p;
        if (parentComponent) { parentComponent->childModified(this); }
        modified();
    }

    void Component::setSize(Point const& s) {
        this->localTransform.size = s;
        if (parentComponent) { parentComponent->childModified(this); }
        modified();
    }
    
    void Component::setRotation(const Float32 r) {
        this->localTransform.rotation = r;
        if (parentComponent) { parentComponent->childModified(this); }
        modified();
    }
    
    void Component::setTransform(Transform const& t) {
        this->localTransform = t;
        if (parentComponent) { parentComponent->childModified(this); }
        modified();
    }

    void Component::setColor(LinearColor const& c) {
        this->renderColor = c;
    }

    Transform Component::globalTransform() const {

        Transform t = this->localTransform;
        const Component* comp = this->parentComponent;

        while (comp) {
            t = t.mix(comp->localTransform);
            comp = comp->parentComponent;
        }
        
        return t;
    }

    UInt Component::depth() const {

        UInt depth = 0;
        
        const Component* p = parentComponent;

        while (p) { p = p->parentComponent; ++depth; }

        return depth;    
        
    }

    bool Component::inBounds(Point const& p) const {
        
        const Transform t = globalTransform();
        const Point pos = t.position;

        return p.x >= pos.x && p.x <= pos.x + t.size.x && p.y >= pos.y && p.y <= pos.y + t.size.y;
        
    }

    Component::~Component() = default;

    void Component::childModified(Component* component) {
    }

    Component::Factory::~Factory() {

        while (components.size() > 0) {
            
            auto* highest = components[0];

            while (highest->parent()) {
                highest = highest->parent();
            }
            
            highest->dispose();
            
        }
        
    }
    
    bool Component::Factory::valid(Component* component) { return instance()->components.contains(component); }
    
    void Component::Factory::dispose(Component* component) {

        if(!valid(component)) return;

        if(component->parentComponent) {
            component->parentComponent->childWillDispose(component);
            component->parentComponent->childrenArray -= component;
        }

        Console::log("Disposing {}.\n", Type::name(*component));
        
        instance()->components.remove(component);
        
    }

    Component::Factory* Component::Factory::instance() {
            
        if (!factory.isValid()) { factory = Unique<Factory>::make(); }
        return factory.pointer();
            
    }
    
}

