#pragma once

#include <Windows.h>
#include <GLFW/glfw3.h>

#include <Flux/Core/Flux.h>
#include <Flux/Core/Memory/UniquePointer.h>
#include <Flux/Core/Tools/Console/Console.h>
#include <Flux/Core/Tools/Console/CommandContext.h>

#include "Flux/UI/UserInterface.h"

namespace Flux {
    
    class Application {
    
    public:

        void run();

        void stop();

        void update();

        void draw();

        static void onError(Int errorCode, const char* description);

        static void onApplicationTerminate();
        
    private:

        void registerCommands();

        bool shouldRun = true;

        UniquePointer<Console> console = nullptr;
        
        GLFWwindow* mainWindow = nullptr;

        SharedPointer<UserInterface::MasterView> masterView;
        
        static inline Application* instance = nullptr;
        
    };
    
}

