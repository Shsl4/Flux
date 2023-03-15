#include <Application/Application.h>

#include <Application/DragComponent.h>
#include <Application/Node.h>
#include <Application/Socket.h>

#include <Application/LinkResolver.h>
#include <Application/FilterGraph.h>

#include "Application/DropdownMenu.h"

using namespace Flux::UI;

namespace Flux {

    class RotaryKnob : public Component {

    public:
        
        void drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override {

            rotation = Math::clamp(rotation + deltaY * 0.35, -120.0, 120.0);

            Console::log("{} {}\n", rotation, deltaY);
            
        }

        void doubleClick(MouseButton button, Float64 x, Float64 y) override {

            if(button == MouseButton::Left) {
                rotation = 0.0;
            }
            
        }

        RotaryKnob() : Component({ {}, {50, 50} }) {
            
            setColor(Color::fromHex(0x3b3b3bff));
            
        }

        static inline auto inner1 = Color::fromHex(0xadaaa7ff);
        static inline auto inner2 = Color::fromHex(0x797979ff);

        void draw(SkCanvas* canvas, Float64 deltaTime) override {

            SkPaint paint;
            const Transform t = globalTransform();
            const Point& pos = t.position;
            const Point& s = t.size;
            const Point mid = t.centeredPosition();
            const Float32 rad = s.x / 2.0f;
            
            paint.setColor(color().skColor());
            canvas->save();
            canvas->rotate(f32(rotation), mid.x, mid.y);
            canvas->drawCircle(mid.x, mid.y, rad, paint);
            paint.setColor(SK_ColorWHITE);
            canvas->drawCircle(mid.x, mid.y, rad * 0.9f, paint);
            paint.setColor(inner1.skColor());
            canvas->drawCircle(mid.x, mid.y, rad * 0.85f, paint);
            paint.setColor(inner2.skColor());
            canvas->drawCircle(mid.x, mid.y, rad * 0.8f, paint);
            paint.setColor(color().skColor());
            canvas->drawCircle(mid.x, mid.y, rad * 0.75f, paint);
            paint.setStrokeCap(SkPaint::kRound_Cap);
            paint.setStrokeWidth(rad * 0.05f);
            paint.setColor(SK_ColorWHITE);
            canvas->drawLine(mid.x, mid.y, pos.x + s.x / 2.0f, pos.y + s.y * 0.1f, paint);
            canvas->restore();
            
        }

    private:

        Float64 rotation = 0.0;
        
    };

    void Application::run() {

        if(instance) throw Exceptions::Exception( "Tried to run multiple Applications at the same time");

        instance = this;

        this->console = Unique<Console>::make();

        glfwSetErrorCallback(&Application::onError);

        if(!glfwInit()){
            throw Exceptions::Exception( "Failed to initialize GLFW");
        }

        constexpr Int windowWidth = 1280;
        constexpr Int windowHeight = 720;

        this->mainWindow = Window::Factory::createWindow(RenderBackend::Metal, "Application", windowWidth, windowHeight);
        
        initializeAudio();

        for(size_t i = 0; i < 10; ++i){
            auto e = this->mainWindow->mainView()->addChild(Component::Factory::create<DragComponent>());
            e->setSize({ 50, 50 });
            e->setColor(Color::randomColor());
        }

        this->shouldRun = true;
        
        Console::log("Created new Flux Application.\n");

        update();

        Console::log("Exiting Application.\n");

        glfwMakeContextCurrent(nullptr);
        
        Window::Factory::activeWindows.clear();

        glfwTerminate();

    }

    void Application::stop() { shouldRun = false; }

    void Application::draw() {
        
        const Float64 currentTime = glfwGetTime();
        const Float64 deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        for(auto& window : Window::Factory::windows()){
            window->draw(deltaTime);
        }

    }

    void Application::initializeAudio() {

        this->audioDevice = Shared<Engine>::make();

        audioDevice->initialize(44100, 128);

    }

    void Application::update() {

        while (shouldRun && !glfwWindowShouldClose(mainWindow->handle)) {
            
            glfwPollEvents();
            draw();

        }

    }

    void Application::onError(Int errorCode, const char* description) {
        Console::error("GLFW Error {}: {}\n", errorCode, description);
    }

    void Application::onApplicationTerminate() { Console::log("Application received termination signal\n"); }

    void Application::registerCommands() {
/*
        const auto stopNode = CommandNode::make("Application.Stop");

        stopNode->setNodeDescription("Exits the application.");
        stopNode->addExecutable([](auto) { instance->stop(); });

        Console::registerCommand(stopNode);*/

    }

}
