#include <Components/Compound.h>

namespace Flux::UserInterface {

    void Compound::draw(SkCanvas* canvas) {

        Component::draw(canvas);

        for (auto const& child :children) {

            child->draw(canvas);
            
        }
        
    }

    void Compound::internalAddChild(SharedPointer<Component> const& drawable) {

        children += drawable;
        drawable->initialize();
        
    }
    
}
