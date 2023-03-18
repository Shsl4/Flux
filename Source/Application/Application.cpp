#include <Application/Application.h>

#include <Application/DragComponent.h>
#include <Application/Node.h>
#include <Application/Socket.h>
#include <Application/LinkResolver.h>
#include <Application/FilterGraph.h>
#include <Application/DropdownMenu.h>
#include <UI/Text.h>
#include <UI/Stack.h>
#include <UI/FrameInfo.h>

using namespace Flux;

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

        this->mainWindow = Factory::createWindow(RenderBackend::Best, "Application", windowWidth, windowHeight);

        auto* stack = Factory::createComponent<VStack>(VAlignment::center, HAlignment::center);
        auto* stack2 = Factory::createComponent<HStack>(VAlignment::center, HAlignment::center);
        auto* stack3 = Factory::createComponent<HStack>(VAlignment::center, HAlignment::center);
        auto* stack4 = Factory::createComponent<HStack>(VAlignment::center, HAlignment::center);
        auto* stack5 = Factory::createComponent<HStack>(VAlignment::center, HAlignment::center);

        stack->setSpacing(50);
        stack2->setColor(Color::randomColor());
        stack3->setColor(Color::randomColor());
        stack4->setColor(Color::randomColor());
        stack5->setColor(Color::randomColor());

        auto* text = Factory::createComponent<TextRenderer>("Hello, world!", 50);
        auto* text2 = Factory::createComponent<TextRenderer>("c[0] = -7", 50);
        auto* text3 = Factory::createComponent<TextRenderer>("Text!", 50);
        auto* text4 = Factory::createComponent<TextRenderer>("[{}] + - _ = & * !", 50);

        this->mainWindow->mainView()->addChild(stack);

        stack->addChild(stack2);
        stack->addChild(stack3);
        stack->addChild(stack4);
        stack->addChild(stack5);

        stack2->addChild(text);
        stack3->addChild(text2);
        stack4->addChild(text3);
        stack5->addChild(text4);

        constexpr Float32 fw = 300.0f;
        constexpr Float32 fh = 30.0f;

        Point p = { f32(windowWidth) - fw - 10.0f, f32(windowHeight) - fh - 10.0f};
        Point s = {fw, fh };

        mainWindow->mainView()->addChild(Flux::Factory::createComponent<FrameInfo>(p, s));

        initializeAudio();

        this->shouldRun = true;
        
        Console::log("Created new Flux Application.\n");

        update();

        Console::log("Exiting Application.\n");

        glfwMakeContextCurrent(nullptr);

        this->factory = nullptr;

        glfwTerminate();

    }

    void Application::stop() { shouldRun = false; }

    void Application::draw() {
        
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
            
            glfwPollEvents();
            draw();

        }

    }

    void Application::onError(Int errorCode, const char* description) {
        Console::error("GLFW Error {}: {}\n", errorCode, description);
    }

    void Application::onApplicationTerminate() { Console::log("Application received termination signal\n"); }

    void Application::registerCommands() {

    }

}
