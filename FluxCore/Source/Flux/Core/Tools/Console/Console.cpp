#include <Tools/Console/Console.h>
#include <Tools/Console/CommandContext.h>

using namespace Flux;

const ConsoleColor ConsoleColor::black = "\33[30m";
const ConsoleColor ConsoleColor::red = "\33[31m";
const ConsoleColor ConsoleColor::green = "\33[32m";
const ConsoleColor ConsoleColor::yellow = "\33[33m";
const ConsoleColor ConsoleColor::blue = "\33[34m";
const ConsoleColor ConsoleColor::magenta = "\33[35m";
const ConsoleColor ConsoleColor::cyan = "\33[36m";
const ConsoleColor ConsoleColor::lightGray = "\33[37m";
const ConsoleColor ConsoleColor::darkGray = "\33[90m";
const ConsoleColor ConsoleColor::lightRed = "\33[91m";
const ConsoleColor ConsoleColor::lightGreen = "\33[92m";
const ConsoleColor ConsoleColor::lightYellow = "\33[93m";
const ConsoleColor ConsoleColor::lightBlue = "\33[94m";
const ConsoleColor ConsoleColor::lightMagenta = "\33[95m";
const ConsoleColor ConsoleColor::lightCyan = "\33[96m";
const ConsoleColor ConsoleColor::reset = "\33[0m";

Console::Console() {
    
    fassertf(!instance, "Console has been built twice. Aborting.");
    
    Console::instance = this;
    
    this->commandTree = UniquePointer<CommandTree>::make();

    const auto helpNode = CommandNode::make("help");
    const auto abortNode = CommandNode::make("abort");

    helpNode->setNodeDescription("Prints this help menu.");
    abortNode->setNodeDescription("Terminates the program in an unsafe way.");
        
    helpNode->addExecutable([this](const auto* context) {
        consoleHelp(context);
    });
    
    abortNode->addExecutable([this](const auto* context) {
        fabort("Explicit termination requested.");
    });
    
    registerCommand(helpNode);
    registerCommand(abortNode);
    
}

Console::~Console(){
    Console::instance = nullptr;
}

void Console::registerNode(SharedPointer<CommandNode> const& node) const
{
    commandTree->registerNode(node);
}

void Console::init() {
    
    runPrompt = true;

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    this->consoleThread = std::thread(&Console::consoleLoop, this);
    this->consoleThread.detach();
    
}

NODISCARD String prompt() {

    std::string input;
    std::getline(std::cin, input);

    if(input.empty())
    {
        std::cout << String::format("\033[A> ");
    }
        
    return input;
}


void Console::consoleLoop() const {
    
    while (runPrompt) {
        
        String line = prompt();

        if(!line.isEmpty())
        {
            execute(line);
        }        

    }
    
}

void Console::execute(String const& command) const {

    Array<String> args = command.split(' ');
    const String name = args.getAt(0);
    args.removeAt(0);
        
    try
    {
        commandTree->execute(name, args);
    }
    catch(CommandError const& error)
    {
        logError(error.what());
    }
    
}

void Console::consoleHelp(const CommandContext*) const {

    logInfo("Printing help: ");

    const auto nodes = commandTree->getNodes();
    
    for (auto const& node : nodes)
    {
        logInfo("{}: {}", node->getNodeName(), node->getNodeDescription());
    }
    
}
