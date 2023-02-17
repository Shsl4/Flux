#include <Components/Compound.h>

#include "CursorManager.h"

namespace Flux::UserInterface {

    Component* Compound::getComponentAtPosition(SkVector const& value) {
        
        for (size_t i = children.size(); i > 0; --i) {

            auto const& child = children[i - 1];

            if (auto* compound = dynamic_cast<Compound*>(child)) {
                
                if(auto* comp = compound->getComponentAtPosition(value)) {
                    
                    return comp;
                    
                }
                
            }

            if(child->isInBounds(value)) {
                
                return child;
                
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

        Int size = i32(children.size());
        
        for (Int i = 0; i < size; ++i) {

            auto const& child = children[i];

            if(child->pendingDiscard) {
                CursorManager::getCursorManager()->notifyDestruction(child);
                child->cleanup();
                children.removeAt(i);
                --size;
                --i;
                continue;
            }
            
            child->draw(canvas, deltaTime);
            
        }

        
    }

    Component* Compound::internalAddChild(Component* component) {

        children += component;
        component->parent = this;
        component->initialize();
        return component;
        
    }
    
}

/*
template<>
class Flux::Formatter<SkVector> {

public:
    static void format(std::ostream& stream, SkVector const& elem) {
        stream << "{ x: " << elem.x() << ", y: " << elem.y() << " }";
    }
        
};
*/