#include <Application/Application.h>

#include <skia/core/SkDrawable.h>
#include <skia/core/SkPath.h>
#include <skia/core/SkRRect.h>
#include <skia/core/SkFont.h>

#include <Application/DragComponent.h>
#include <Application/Node.h>
#include <Application/Socket.h>

#include <Application/LinkResolver.h>
#include <Application/FilterGraph.h>

#define GL_FRAMEBUFFER_BINDING 0x8CA6

namespace Flux {
    
    class FrameInfo : public UserInterface::Component {

    public:
        
        void initialize() override {

            setScale({300, 30});
            setColor(UserInterface::LinearColor::fromHex(0x60606060));

        }
        
        void draw(SkCanvas* canvas, Float64 deltaTime) override {

            const Float64 currentTime = glfwGetTime();
            ++frameCount;

            if(currentTime - lastTime >= 1.0) {

                lastFrameTime = (1000.0 / static_cast<Float64>(frameCount));
                frameCount = 0;
                lastTime += 1.0;
                
            }
            
            SkPaint paint;
            
            paint.setColor(getColor().toSkColor());

            const SkVector pos = getAbsolutePosition();
            SkRect rect = SkRect::MakeXYWH(pos.x(), pos.y(), getScale().x(), getScale().y());

            canvas->drawRoundRect(rect, 10, 10, paint);
            
            paint.setColor(SK_ColorWHITE);

            String text = String::format("Draw: {} FPS ({}ms)", 1000.0 * (1.0 / lastFrameTime), lastFrameTime);
            SkFont font;
            font.setSize(12.0);

            canvas->drawSimpleText(text.begin().get(), text.size(), SkTextEncoding::kUTF8, pos.fX + 10.0f,
                                   pos.fY + 20.0f, font, paint);

            
        }

    private:

        Float64 lastTime = 0.0;
        UInt frameCount = 0;
        Float64 lastFrameTime = 0.0;
        
    };

    class RotaryKnob : public UserInterface::Component {

    public:
        
        void onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override {

            rotation = Math::clamp(rotation + deltaY * 0.35, -120.0, 120.0);

            Console::log("{} {}\n", rotation, deltaY);
            
        }

        void onDoubleClick(UserInterface::MouseButton button, Float64 x, Float64 y) override {

            if(button == UserInterface::MouseButton::Left) {
                rotation = 0.0;
            }
            
        }

        void initialize() override {

            setScale({ 50, 50 });
            setColor(UserInterface::LinearColor::fromHex(0x3b3b3bff));
            
        }
        
        static inline auto inner1 = UserInterface::LinearColor::fromHex(0xadaaa7ff);
        static inline auto inner2 = UserInterface::LinearColor::fromHex(0x797979ff);

        void draw(SkCanvas* canvas, Float64 deltaTime) override {

            SkPaint paint;
            const SkVector pos = getAbsolutePosition();
            const SkVector size = getScale();
            const SkVector mid = getAbsoluteCenteredPosition();
            const Float32 rad = size.x() / 2.0f;
            paint.setColor(getColor().toSkColor());
            canvas->save();
            canvas->rotate(f32(rotation), mid.fX, mid.fY);
            canvas->drawCircle(mid, rad, paint);
            paint.setColor(SK_ColorWHITE);
            canvas->drawCircle(mid, rad * 0.9f, paint);
            paint.setColor(inner1.toSkColor());
            canvas->drawCircle(mid, rad * 0.85f, paint);
            paint.setColor(inner2.toSkColor());
            canvas->drawCircle(mid, rad * 0.8f, paint);
            paint.setColor(getColor().toSkColor());
            canvas->drawCircle(mid, rad * 0.75f, paint);
            paint.setStrokeCap(SkPaint::kRound_Cap);
            paint.setStrokeWidth(rad * 0.05f);
            paint.setColor(SK_ColorWHITE);
            canvas->drawLine(mid.fX, mid.fY, pos.fX + size.fX / 2.0f, pos.fY + size.fY * 0.1f, paint);
            canvas->restore();
            
        }

    private:

        Float64 rotation = 0.0;
        
    };

    void Application::run() {

        if(instance) throw Exceptions::Exception( "Tried to run multiple Applications at the same time");

        instance = this;

        this->console = Unique<Console>::make();
/*
        console->init();

        setTerminationCallback(&Application::onApplicationTerminate);

        registerCommands();*/

        glfwSetErrorCallback(&Application::onError);

        if(!glfwInit()){
            throw Exceptions::Exception( "Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_STENCIL_BITS, 8);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_DEPTH_BITS, 0);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        constexpr Int windowWidth = 1280;
        constexpr Int windowHeight = 720;
        
        this->mainWindow = glfwCreateWindow(windowWidth, windowHeight, "Application", nullptr, nullptr);

        if(!mainWindow){
            throw Exceptions::Exception("Failed to create window");
        }

        glfwMakeContextCurrent(mainWindow);

        glfwSwapInterval(1);

        glfwSetKeyCallback(mainWindow, &Application::inputCallback);
        glfwSetMouseButtonCallback(mainWindow, &Application::mouseCallback);
        glfwSetScrollCallback(mainWindow, &Application::scrollCallback);
        glfwSetCursorPosCallback(mainWindow, &Application::cursorCallback);
        
        Int framebufferWidth;
        Int framebufferHeight;

        glfwGetFramebufferSize(mainWindow, &framebufferWidth, &framebufferHeight);

        Int32 framebufferId = 0;
        const auto glGetIntegerv = reinterpret_cast<void(*)(UInt32, Int32*)>(glfwGetProcAddress("glGetIntegerv"));
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebufferId);
        
        this->masterView = UserInterface::MasterView::makeGL(framebufferWidth, framebufferHeight, {framebufferId, 4, 8, f32(framebufferWidth) / f32(windowWidth), f32(framebufferHeight) / f32(windowHeight)});

        initializeAudio();

        this->cursorManager = Shared<UserInterface::CursorManager>::make();
        cursorManager->setCompound(masterView.pointer());

        const auto frameInfo = masterView->addChild<FrameInfo>();
        frameInfo->setPosition({ f32(windowWidth) - frameInfo->getScale().fX - 10.0f, f32(windowHeight) - frameInfo->getScale().fY - 10.0f});
        
        this->shouldRun = true;

        this->masterView->addChild<LinkResolver>()->pipeline = audioDevice->getPipeline();

        Console::log("Created new Flux Application.\n");

        update();

        Console::log("Exiting Application.\n");

        glfwMakeContextCurrent(nullptr);
        glfwDestroyWindow(mainWindow);
        glfwTerminate();

    }

    void Application::stop() { shouldRun = false; }

    void Application::draw() {
        
        const Float64 currentTime = glfwGetTime();
        const Float64 deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        masterView->draw(nullptr, deltaTime);
        glfwSwapBuffers(mainWindow);

    }

    void Application::initializeAudio() {

        this->audioDevice = Shared<MyAudioDevice>::make();

        audioDevice->initialize(44100, 128);

    }

    void Application::update() {
        while (shouldRun && !glfwWindowShouldClose(mainWindow)) {
            
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
