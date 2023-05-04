#include <UI/Component.h>
#include <Flux/Factory.h>

namespace Flux {

    const Point Point::zero = {};

    Component::Component(Transform const& t): localTransform(t) {

    }

    Component::Component(Point const& p, Point const& s) : localTransform({ p, s, 0.0f }) {

    }

    void Component::draw(Graphics const& graphics) {

        if(!visible()) { return; }

        if(!color().transparent()){

            graphics.setColor(renderColor);
            graphics.drawRect(globalTransform());

        }

        for(const auto& child : childrenArray) {
            child->draw(graphics);
        }
        
    }
    
    Component* Component::addChild(Component* component) {

        nthrowif(!Factory::valid(this), "Trying to add child on an invalid component! "
                                        "Make sure it is initialized, and use Component::Factory::create() to createComponent components.");

        nthrowif(!Factory::valid(component), "Trying to add an invalid component! "
                                             "Use Component::Factory::create() to createComponent components.");

        nthrowif(component->parentComponent, "Trying to add a component that already has a parent!");

        nthrowif(component == this, "Trying to add a component to itself!");

        nthrowif(!supportsChildren(), "Trying to add a child to a component that does not support children!");

        component->parentComponent = this;
        childrenArray += component;
        
        component->parentLinked();
        childAdded(component);

        return component;
        
    }

    void Component::initialize() {
        
    }

    void Component::dispose() {

        willDispose();

        for (size_t i = childrenArray.size(); i > 0; --i) {
            childrenArray[i - 1]->dispose();
        }

        if(parentComponent) {
            parentComponent->childrenArray -= this;
            parentComponent->childWillDispose(this);
        }

        parentComponent = nullptr;

        Factory::dispose(this);

    }

    void Component::setPosition(Point const& p) {
        this->localTransform.position = p;
        if (parentComponent) { parentComponent->childModified(this); }
        for (auto& child : childrenArray) { child->parentModified(); }
        modified();
    }

    void Component::setSize(Point const& s) {
        this->localTransform.size = s;
        if (parentComponent) { parentComponent->childModified(this); }
        for (auto& child : childrenArray) { child->parentModified(); }
        modified();
    }
    
    void Component::setRotation(const Float32 r) {
        this->localTransform.rotation = r;
        if (parentComponent) { parentComponent->childModified(this); }
        for (auto& child : childrenArray) { child->parentModified(); }
        modified();
    }
    
    void Component::setTransform(Transform const& t) {
        this->localTransform = t;
        if (parentComponent) { parentComponent->childModified(this); }
        for (auto& child : childrenArray) { child->parentModified(); }
        modified();
    }

    void Component::setColor(Color const& c) {
        this->renderColor = c;
        colorChanged();
    }

    void Component::setVisible(const bool state) {
        this->bVisible = state;
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

    bool Component::supportsChildren() const {
        return true;
    }

    void Component::disposeAllChildren() {

        while (childrenArray.size() > 0){
            childrenArray[0]->dispose();
        }
        
    }

    bool Component::isChild(const Component* component) const {
        
        for (auto const& child : childrenArray) {
            if (component == child) return true;
        }

        return false;

    }

    bool Component::isNestedChild(const Component* component) const {

        if(isChild(component)) return true;

        for (auto const& child : childrenArray) {

            if(child->isNestedChild(component)) return true;
            
        }

        return false;
        
    }

    Window* Component::window() const {

        for (auto const& window : Factory::windows()) {

            if(window->mainComponent() == this || window->mainComponent()->isNestedChild(this)) return window;
            
        }

        return nullptr;
        
    }

    Component* Component::componentAtPosition(const Point &p) {

        if(!reactive()) return nullptr;
        
        for(size_t i = childrenArray.size(); i > 0; --i){
            
            const auto& child = childrenArray[i - 1];

            if(auto* component = child->componentAtPosition(p)) return component;

        }

        return inBounds(p) ? this : nullptr;

    }

}

