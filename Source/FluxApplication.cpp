#include <Application/Application.h>

using namespace Flux;

int main() {

    ExceptionHandler::run([] {

        Application().run();
        
    });
        
}
