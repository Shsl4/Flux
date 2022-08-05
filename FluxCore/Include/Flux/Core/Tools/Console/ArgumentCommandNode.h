#pragma once

#include <Flux/Core/Tools/Console/CommandNode.h>

namespace Flux {
    
    class ArgumentCommandNode final : public CommandNode {

    public:

        ArgumentCommandNode() = delete;

        explicit ArgumentCommandNode(String name, ArgumentType type);

        static SharedPointer<ArgumentCommandNode> make(String const& name, ArgumentType type);

        FORCEINLINE ArgumentType getArgumentType() const {
            return this->argumentType;
        }
        
    private:
    
        ArgumentType argumentType;
    
    };

}