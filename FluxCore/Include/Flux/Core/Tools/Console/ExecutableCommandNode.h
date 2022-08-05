#pragma once

#include <Flux/Core/Tools/Console/CommandNode.h>

namespace Flux {
    
    class ExecutableCommandNode final : public CommandNode {

    public:

        ExecutableCommandNode() = delete;

        ExecutableCommandNode(String name, CommandFunction function);

        static SharedPointer<ExecutableCommandNode> make(String const& name, CommandFunction const& function);

        FORCEINLINE CommandFunction getCommandFunction() const
        {
            return this->function;
        }

    private:
        
        CommandFunction function;
    
    };

}