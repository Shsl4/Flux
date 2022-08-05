#include "Application.h"

namespace Flux {
    
    void Application::run() {
        
        fassertf(!instance, "Tried to run multiple Applications at the same time");

        instance = this;

        this->console = UniquePointer<Console>::make();

        console->init();

        registerCommands();

        setTerminationCallback(&Application::onApplicationTerminate);
        
        glfwSetErrorCallback(&Application::onError);
        
        fassertf(glfwInit(), "Failed to initialize GLFW");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_DOUBLEBUFFER , GLFW_TRUE);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_STENCIL_BITS, 0);
        glfwWindowHint(GLFW_SAMPLES, 0);
        glfwWindowHint(GLFW_DEPTH_BITS, 0);

        this->mainWindow = glfwCreateWindow(1280, 720, "Application", nullptr, nullptr);

        fassertf(this->mainWindow, "Failed to create window");

        glfwMakeContextCurrent(mainWindow);

        GLint drawFboId = 0;
        HMODULE libGL = LoadLibraryW(L"opengl32.dll");
        void(*e)(GLenum, Int*) = (void(*)(GLenum, Int*))GetProcAddress(libGL, "glGetIntegerv");
        e(0x8CA6, &drawFboId);
        FreeLibrary(libGL);
        
        this->masterView = UserInterface::MasterView::makeGL(1280, 720, { drawFboId, 0, 0});
        
        this->shouldRun = true;
        
        Console::logStatus("Created new Flux Application.");

        update();

        Console::logStatus("Exiting Application.");
        
        glfwMakeContextCurrent(nullptr);
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        
    }

    void Application::stop() {
        shouldRun = false;
    }

    void Application::draw() {

        masterView->draw();
        glfwSwapBuffers(mainWindow);
        
    }

    void Application::update() {

        while (shouldRun && !glfwWindowShouldClose(mainWindow)) {
            
            glfwPollEvents();
            draw();
        
        }
        
    }

    void Application::onError(Int errorCode, const char* description) {

        Console::logRuntime("GLFW Error {}: {}", errorCode, description);
        
    }

    void Application::onApplicationTerminate() {

        Console::logFatal("Application received termination signal");
        
    }

    void Application::registerCommands() {

        const auto stopNode = CommandNode::make("Application.Stop");

        stopNode->setNodeDescription("Exits the application.");
        stopNode->addExecutable([](auto) { instance->stop(); });

        Console::registerCommand(stopNode);
        
    }


}
