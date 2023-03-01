#pragma once

#include <GLFW/glfw3.h>

#include <Audio/MyAudioDevice.h>
#include <Flux/UI/CursorManager.h>
#include <Flux/UI/UserInterface.h>

namespace Flux {
    
    class AudioDevice;

    class Application {

    public:

        void run();

        void stop();

        NODISCARD static UI::Component* mainView() { return instance->masterView; }
        
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
        
        static void registerCommands();

        bool shouldRun = true;

        Float64 lastTime = 0.0;

        Float64 lastClickTime = 0.0;
        
        GLFWwindow* mainWindow = nullptr;

        Unique<Console> console = nullptr;
        
        UI::Component* masterView = nullptr;
        
        Shared<MyAudioDevice> audioDevice = nullptr;
        
        Shared<UI::CursorManager> cursorManager = nullptr;
        
        UI::MouseButton lastButtonPressed = UI::MouseButton::Size;
        
        static inline Application* instance = nullptr;
        
    };
    
}

