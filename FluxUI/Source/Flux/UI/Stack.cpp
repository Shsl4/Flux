#include <Flux/UI/Stack.h>

namespace Flux::UI {
    
    void Stack::childAdded(Component* child) {
        realign();
    }

    void Stack::realign() {
        
        if (this->alignment == Alignment::Horizontal) {
            
            const auto& ch = children();

            Float32 lastX = 0.0f;
            
            for(auto* c : ch) {

                const auto pos = c->position();
                const auto s = c->size();
                const Float32 newWidth = s.x > size().x ? size().x : s.x;
                const Float32 newHeight = s.y > size().y ? size().y : s.y;

                c->setTransform({ { lastX, pos.y }, { newWidth, newHeight }, 0.0f });
                
                lastX += s.x;
                
            }
            
        }
        
    }
}

