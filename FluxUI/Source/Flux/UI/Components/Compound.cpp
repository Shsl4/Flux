#include <Components/Compound.h>

#include "CursorManager.h"

namespace Flux::UserInterface {

    Component* Compound::getComponentAtPosition(SkVector const& value) {
        
        for (size_t i = children.getSize(); i > 0; --i) {

            auto const& child = children[i - 1];

            if (auto* compound = dynamic_cast<Compound*>(child.raw())) {
                
                if(auto* comp = compound->getComponentAtPosition(value)) {
                    
                    return comp;
                    
                }
                
            }

            if(child->isInBounds(value)) {
                
                return child.raw();
                
            }
            
        }

        if(isInBounds(value)) {
            return this;
        }
        
        return nullptr;
        
    }

    Component* Compound::getChild(size_t index) const {
        return children[index];
    }

    void Compound::cleanup() {

        Component::cleanup();

        for (const auto& child : children) {
            child->cleanup();
        }
        
    }

    void Compound::draw(SkCanvas* canvas, Float64 deltaTime) {

        Int size = i32(children.getSize());
        
        for (Int i = 0; i < size; ++i) {

            auto const& child = children[i];

            if(child->pendingDiscard) {
                CursorManager::getCursorManager()->notifyDestruction(child.raw());
                child->cleanup();
                children.removeAt(i);
                --size;
                --i;
                continue;
            }
            
            child->draw(canvas, deltaTime);
            
        }

        
    }

    void Compound::internalAddChild(SharedPointer<Component> const& component) {

        children += component;
        component->parent = this;
        component->initialize();
        
    }
    
}

template<>
class Flux::Formatter<SkVector> {

public:
    static void format(std::ostream& stream, SkVector const& elem) {
        stream << "{ x: " << elem.x() << ", y: " << elem.y() << " }";
    }
        
};
