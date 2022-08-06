#pragma once

#include <GLFW/glfw3.h>

#include <Flux/Core/Flux.h>
#include <Flux/Core/Memory/UniquePointer.h>
#include <Flux/Core/Tools/Console/Console.h>

#include "Audio/MyAudioDevice.h"
#include "Flux/UI/UserInterface.h"

namespace Flux {
    
    class AudioDevice;

    class Application {
    
    public:

        void run();

        void stop();

        void update();

        void draw();

        void initializeAudio();
        
        static void onError(Int errorCode, const char* description);

        static void onApplicationTerminate();
        
    private:
        
        static void inputCallback(GLFWwindow* window, Int key, Int scancode, Int action, Int mods);
        static void mouseCallback(GLFWwindow* window, Int button, Int action, Int mods);
        
        void registerCommands();

        bool shouldRun = true;

        UniquePointer<Console> console = nullptr;
        
        GLFWwindow* mainWindow = nullptr;

        SharedPointer<UserInterface::MasterView> masterView = nullptr;
        
        SharedPointer<MyAudioDevice> audioDevice = nullptr;
        
        static inline Application* instance = nullptr;
        
    };
    
}

