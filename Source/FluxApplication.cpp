#include <Application/Application.h>
#include <Application/WaveFile.h>

using namespace Flux;

int main() {

    ExceptionHandler::run([] {

        Application().run();
        
    });
        
}
