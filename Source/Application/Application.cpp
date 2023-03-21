#define _CRT_SECURE_NO_WARNINGS
#include <Application/Application.h>

#include <Application/DragComponent.h>
#include <Application/Node.h>
#include <Application/Socket.h>
#include <Application/LinkResolver.h>
#include <Application/FilterGraph.h>
#include <UI/DropdownMenu.h>
#include <UI/Text.h>
#include <UI/Stack.h>
#include <UI/FrameInfo.h>
#include <UI/Button.h>
#include <Examples/ColorsExample.h>
#include <Examples/HelloWorld.h>
#include <Examples/StacksExample.h>

using namespace Flux;

namespace Flux {

    void Application::run() {

        if(application) throw Exceptions::Exception( "Tried to run multiple Applications at the same time");

        application = this;

        auto* errorFile = freopen("LibErrors.log", "w", stderr);

        this->console = Unique<Console>::make();
        this->factory = Unique<Factory>::make();

        glfwSetErrorCallback(&Application::onError);

        if(!glfwInit()){
            throw Exceptions::Exception("Failed to initialize GLFW");
        }

        constexpr Int windowWidth = 1280;
        constexpr Int windowHeight = 720;

        this->mainWindow = Factory::createWindow(RenderBackend::Best, "Application", windowWidth, windowHeight);
/*
        auto stack = Factory::createComponent<ColorsExample>(Point::zero, Point(windowWidth, windowHeight));

        this->mainWindow->rootComponent->addChild(stack);

*/
        auto stack = Factory::createComponent<VStack>(Point::zero, Point(windowWidth, windowHeight), VAlignment::top, HAlignment::center);

        this->mainWindow->rootComponent->addChild(stack);

        auto* component = Factory::createComponent<DropdownMenu>(Point::zero, Point(200, 25));

        stack->addChild(component);

        MutableArray<String> data;

        for (size_t i = 0; i < 10; ++i) {
            data += String::format("Choice {}", i + 1);
        }

        component->setChoices(data);


        constexpr Float32 fw = 100.0f;
        constexpr Float32 fh = 100.0f;

        Point s = {fw, fh };

        mainWindow->mainView()->addChild(Flux::Factory::createComponent<FrameInfo>(Point::zero, s));

        //initializeAudio();

        this->shouldRun = true;

        Console::log("Created new Flux Application.\n");

        update();

        Console::log("Exiting Application.\n");

        glfwMakeContextCurrent(nullptr);

        this->factory = nullptr;

        fclose(errorFile);

        glfwTerminate();

    }

    void Application::stop() { shouldRun = false; }

    void Application::draw()  {

        const Float64 currentTime = glfwGetTime();
        const Float64 deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        for(auto& window : Factory::windows()){

            window->draw(deltaTime);

        }

    }

    void Application::initializeAudio() {

        this->audioDevice = Shared<Engine>::make();

        audioDevice->initialize(44100, 128);

    }

    void Application::update() {

        while (shouldRun) {

            this->draw();
            glfwPollEvents();

        }

    }

    void Application::onError(Int errorCode, const char* description) {
        Console::error("GLFW Error {}: {}\n", errorCode, description);
    }

    void Application::onApplicationTerminate() { Console::log("Application received termination signal\n"); }

    void Application::registerCommands() {

    }

}
