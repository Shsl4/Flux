#pragma once

#include <Audio/Engine.h>
#include <Application/Window.h>

namespace Flux {
    
    class AudioEngine;

    class Application {

    public:

        void run();

        void stop();

    private:

        friend class Window;

        void update();

        void draw();

        void initializeAudio();
        
        static void onError(Int errorCode, const char* description);

        static void onApplicationTerminate();
        
        static void registerCommands();

        bool shouldRun = true;

        Float64 lastTime = 0.0;

        Window* mainWindow = nullptr;

        Unique<Console> console = nullptr;

        Shared<Engine> audioDevice = nullptr;

        static inline Application* instance = nullptr;
        
    };
    
}

