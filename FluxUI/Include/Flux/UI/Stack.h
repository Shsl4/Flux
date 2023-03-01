#pragma once

#include <Flux/UI/Component.h>

namespace Flux::UI {
    
    class Stack : public Component {

    public:

        enum class Alignment {
            Vertical,
            Horizontal
        };
        
    protected:
        
        void childAdded(Component* child) override;

    private:
        
        void realign();

        Alignment alignment = Alignment::Horizontal;
    
    };
    
}
