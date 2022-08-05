#pragma once

#include <Flux/Core/Tools/Console/CommandNode.h>
#include <Flux/Core/Memory/SharedPointer.h>

namespace Flux {
    
    class CommandTree {

    public:
        
        CommandTree();

        void execute(String const& command, Array<String> const& args);

        void registerNode(SharedPointer<CommandNode> const& node) const;

        FORCEINLINE WeakPointer<CommandNode> getRootNode() const { return rootNode.weak(); }

        FORCEINLINE OwnedArray<CommandNode> getNodes() const { return rootNode->nodes; }

    private:
        
        static void gatherLeaves(CommandNode* node, Array<CommandNode*>& storage);

        SharedPointer<CommandNode> rootNode;

    };
    
}
