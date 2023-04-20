#include <Application/Application.h>

using namespace Flux;

int main() {
    
    ExceptionHandler::run([] {
        
        Class::fromName("Flux::Application").unique<Application>()->run();
        
    });
        
}
