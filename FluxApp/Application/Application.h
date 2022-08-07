#pragma once

#include <GLFW/glfw3.h>
#include <skia/core/SkFont.h>

#include <Flux/Core/Flux.h>
#include <Flux/Core/Memory/UniquePointer.h>
#include <Flux/Core/Tools/Console/Console.h>

#include "../Audio/MyAudioDevice.h"
#include "Flux/UI/CursorManager.h"
#include "Flux/UI/UserInterface.h"

namespace Flux {
    
    class AudioDevice;

    class Application {

    public:

        void run();

        void stop();
    
    private:
        
        void update();

        void draw();

        void initializeAudio();
        
        static void onError(Int errorCode, const char* description);

        static void onApplicationTerminate();
        
        static void inputCallback(GLFWwindow* window, Int key, Int scancode, Int action, Int mods);
        
        static void mouseCallback(GLFWwindow* window, Int button, Int action, Int mods);
        
        static void scrollCallback(GLFWwindow* window, Float64 xOffset, Float64 yOffset);
        
        static void cursorCallback(GLFWwindow* window, Float64 xPos, Float64 yPos);
        
        void registerCommands();

        bool shouldRun = true;

        Float64 lastTime = 0.0;

        UniquePointer<Console> console = nullptr;
        
        GLFWwindow* mainWindow = nullptr;

        SharedPointer<UserInterface::MasterView> masterView = nullptr;
        
        SharedPointer<MyAudioDevice> audioDevice = nullptr;
        
        SharedPointer<UserInterface::CursorManager> cursorManager = nullptr;
        
        static inline Application* instance = nullptr;
        
    };
    
}

