#define _CRT_SECURE_NO_WARNINGS
#include <Application/Application.h>

#include <UI/DropdownMenu.h>
#include <UI/Text.h>
#include <UI/Stack.h>
#include <UI/FrameInfo.h>
#include <UI/Button.h>
#include <Examples/ColorsExample.h>
#include <Examples/HelloWorld.h>
#include <Examples/StacksExample.h>

#include <Application/BodePlot.h>
#include <UI/SceneComponent.h>

#include "Utility/Timer.h"

using namespace Flux;

namespace Flux {

    void Application::run() {

        if(application) throw Exceptions::Exception( "Tried to run multiple Applications at the same time");

        application = this;

        this->console = Unique<Console>::make();
        this->factory = Unique<Factory>::make();

        glfwSetErrorCallback(&Application::onError);

        if(!glfwInit()){
            throw Exceptions::Exception("Failed to initialize GLFW");
        }

        constexpr Int windowWidth = 1280;
        constexpr Int windowHeight = 720;

        /*
        auto* scene = Factory::createComponent<SceneComponent>(Point::zero, Point(windowWidth, windowHeight)); 
        this->mainWindow = Factory::createWindow(scene, RenderBackend::Best, "Application", windowWidth, windowHeight);
        */
        
        constexpr Float32 fw = 100.0f;
        constexpr Float32 fh = 100.0f;

        initializeAudio();
        
        Point s = { fw, fh };

       factory->windows()[0]->mainComponent()->addChild(Factory::createComponent<FrameInfo>(Point::zero, s));
        
        this->shouldRun = true;

        Console::log("Created new Flux Application.\n");

        update();

        Console::log("Exiting Application.\n");

        glfwMakeContextCurrent(nullptr);
        
        this->audioDevice->close();

        this->factory = nullptr;

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
