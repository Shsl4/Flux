#pragma once

#include <Audio/Engine.h>
#include <Application/Window/Window.h>
#include <Flux/Factory.h>

namespace Flux {
    
    class AudioEngine;

    class Application {

    public:

        void run();

        void stop();

        NODISCARD FORCEINLINE static Application* instance() { return application; }

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
        
        Unique<Factory> factory = nullptr;

        Shared<Engine> audioDevice = nullptr;

        static inline Application* application = nullptr;
        
    };
    
}

