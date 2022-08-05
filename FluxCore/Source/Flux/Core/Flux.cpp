#include <Flux.h>
#include <Tools/Console/Console.h>
#include <Tools/StackTrace.h>

static void(*TerminationCallback)();

namespace Flux {
    
    void expectImpl(bool b, const char* f) {

        if(!b) {
            abortImpl(f);
        }
    
    }

    void abortImpl(const char* f) {

        if(TerminationCallback) {
            TerminationCallback();
        }
        
        Console::logFatal("Assertion failed: {}", f);
        
        Console::logFatal("Call stack: ");

        for (const auto symbols = StackTrace::getStackTrace(); auto const& sym : symbols) {

            Console::logFatal(sym.format());
                
        }
        
        abort();        
        
    }

    void setTerminationCallback(void(*callback)()) {
        TerminationCallback = callback;        
    }

}
