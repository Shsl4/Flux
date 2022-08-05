#include <Tools/Console/Console.h>
#include <Tools/Console/ArgumentCommandNode.h>
#include <Tools/Console/CommandTree.h>
#include <Tools/Console/ExecutableCommandNode.h>
#include <Tools/Console/CommandContext.h>
#include <Memory/UniquePointer.h>

using namespace Flux;

CommandTree::CommandTree() { this->rootNode = SharedPointer<CommandNode>::make("root"); }

void CommandTree::execute(String const& command, Array<String> const& args) {

    CommandNode* node = rootNode->findLiteralNode(command);

    if (!node) {
        
        Console::logError("{}: Unknown command.", command);
        return;
    }

    const size_t argc = args.getSize();
    const auto context = UniquePointer<CommandContext>::make();

    for (size_t i = 0; i < argc; ++i) {
        String& arg = args[i];
        CommandNode* nextNode = node->findLiteralNode(arg);

        if (nextNode) {
            node = nextNode;
            continue;
        }

        nextNode = node->findFirstOf(NodeType::Argument);

        if (nextNode) {
            const auto* argNode = dynamic_cast<ArgumentCommandNode*>(nextNode);

            context->tryParse(arg, argNode->getNodeName(), argNode->getArgumentType());

            node = nextNode;

            continue;
        }

        CommandError::throwError("Too many arguments provided. Expected {}, got {}.", i, argc);
        
    }

    node = node->findFirstOf(NodeType::Executable);
    
    if(node) {
        dynamic_cast<ExecutableCommandNode*>(node)->getCommandFunction()(context.raw());
    }
    else {

        const Array<String> paths = rootNode->findLiteralNode(command)->getPaths();
        
        String finalString = "Available paths: \n";

        for (size_t i = 0; i < paths.getSize(); ++i) {
            finalString += String::format("- {}", paths[i]);
            if (i != paths.getSize() - 1) { finalString += String("\n"); }
        }

        CommandError::throwError("This command requires more arguments.\n{}", finalString); 
    }
    
}

void CommandTree::registerNode(SharedPointer<CommandNode> const& node) const {
    
    try {
        
        CommandError::throwIf(node->getNodeName().isEmpty(), "Tried to register an unnamed node.");
        CommandError::throwIf(rootNode->containsNode(node.raw()), "This node is already present in the command tree.");
        CommandError::throwIf(rootNode->findChildNode(node->getNodeName()), "A command named {} already exists.",
                              node->getNodeName());
        CommandError::throwIf(node->nodeType != NodeType::Literal, "You may only register command literals.");

        Array<CommandNode*> leaves;
        gatherLeaves(node.raw(), leaves);

        for (auto const& leaf : leaves) {
            CommandError::throwIf(leaf->nodeType != NodeType::Executable, "One of the leaves is not executable."
                                  " Make sure to bind a function for every command.");
        }

        node->lock();
        rootNode->nodes += node;
    }
    catch (CommandError const& error) {
        
        Console::logError( String::format("Error while registering command {}: {}", node->getNodeName(), error.what()));
        
    }
}

void CommandTree::gatherLeaves(CommandNode* node, Array<CommandNode*>& storage) {

    for (auto const& child : node->nodes) { gatherLeaves(child.raw(), storage); }

    if (node->isLeaf()) { storage += node; }

}
