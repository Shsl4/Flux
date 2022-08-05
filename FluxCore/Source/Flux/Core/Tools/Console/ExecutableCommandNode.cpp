#include <Tools/Console/ExecutableCommandNode.h>

using namespace Flux;

ExecutableCommandNode::ExecutableCommandNode(String name, CommandFunction function):
    CommandNode(std::move(name)), function(std::move(function))
{
    this->nodeType = NodeType::Executable;
}

SharedPointer<ExecutableCommandNode> ExecutableCommandNode::make(String const& name, CommandFunction const& function)
{
    return SharedPointer<ExecutableCommandNode>::make(name, function);
}
