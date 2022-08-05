#pragma once

#include <Flux/Core/Flux.h>
#include <Flux/Core/Memory/String.h>
#include <Flux/Core/Memory/SharedPointer.h>
#include <Flux/Core/Tools/Console/ConsoleTypes.h>

namespace Flux {

    class CommandError : public std::runtime_error {

    public:
        using Super = std::runtime_error;

        template <typename ... Args>
        FORCEINLINE static void throwIf(const bool condition, const char* format, Args&&... args) {
            if (condition) { throw CommandError(String::format(format, args...)); }
        }

        template <typename ... Args>
        NORETURN FORCEINLINE static void throwError(const char* format, Args&&... args) {
            throw CommandError(String::format(format, args...));
        }

    protected:
        CommandError(String const& message) : Super(message.toCString()) {}

        CommandError(const char* message) : Super(message) {}

    };

    class ParseError : public std::runtime_error {

    public:
        using Super = std::runtime_error;

        template <typename ... Args>
        FORCEINLINE static void throwIf(const bool condition, const char* format, Args&&... args) {
            if (condition) { throw ParseError(String::format(format, args...)); }
        }

        template <typename ... Args>
        NORETURN FORCEINLINE static void throwError(const char* format, Args&&... args) {
            throw ParseError(String::format(format, args...));
        }

    protected:
        ParseError(String const& message) : Super(message.toCString()) {}

        ParseError(const char* message) : Super(message) {}

    };

    class CommandNode {

    public:
        using CommandFunction = std::function<void(const class CommandContext*)>;

        CommandNode() = delete;

        virtual ~CommandNode();

        explicit CommandNode(String name);

        static SharedPointer<CommandNode> make(const String& name);

        CommandNode* addLiteral(const String& name);

        CommandNode* addArgument(const String& name, ArgumentType type);

        CommandNode* addExecutable(CommandFunction const& function);

        void lock();

        void setNodeDescription(String const& description);

        NODISCARD CommandFunction getExecutable() const;

        bool containsNode(const CommandNode* node) const;

        void putPath(const CommandNode* node, Array<String>& storage, String string) const;

        NODISCARD Array<String> getPaths() const;

        FORCEINLINE bool isLeaf() const { return this->nodes.getSize() == 0; }

        FORCEINLINE bool isLocked() const { return this->locked; }

        FORCEINLINE String getNodeName() const { return this->nodeName; }

        FORCEINLINE String getNodeDescription() const {
            return this->nodeDescription.isEmpty() ? "No description provided." : this->nodeDescription;
        }

        FORCEINLINE NodeType getNodeType() const { return this->nodeType; }

    protected:
        friend class CommandTree;

        NODISCARD CommandNode* findChildNode(String const& name) const;

        NODISCARD CommandNode* findLiteralNode(String const& name) const;

        NODISCARD CommandNode* findFirstOf(NodeType type) const;

        String nodeName;
        String nodeDescription;
        OwnedArray<CommandNode> nodes;
        NodeType nodeType = NodeType::Executable;

    private:
        bool locked = false;

    };

}
