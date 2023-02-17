#include <Application/Application.h>

int main() {

    ExceptionHandler::run([](){

        Flux::Application().run();

    });

}