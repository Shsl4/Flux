#include <Application/Application.h>

#include <skia/core/SkDrawable.h>
#include <skia/core/SkPath.h>
#include <skia/core/SkRRect.h>
#include <skia/core/SkFont.h>

#include <Application/DragComponent.h>
#include <Application/Node.h>
#include <Application/Socket.h>

namespace Flux {
    
    class FrameInfo : public DragComponent {

    public:
        
        void initialize() override {

            setScale({150, 45});
            setColor(UserInterface::LinearColor::fromHex(0x606060ff));

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

            canvas->drawRect(rect, paint);
            
            paint.setColor(UserInterface::LinearColor::fromHex(0x147ff6ff).toSkColor());
            rect = SkRect::MakeXYWH(pos.x(), pos.y(), getScale().x(), 5.0f);
            canvas->drawRect(rect, paint);
            
            paint.setColor(SK_ColorWHITE);

            String text = String::format("Frame time: {}ms", lastFrameTime);
            SkFont font;
            font.setSize(12.0);

            canvas->drawSimpleText(text.toCString(), text.getSize(), SkTextEncoding::kUTF8, pos.fX + 10.0f,
                                   pos.fY + 30.0f, font, paint);

            
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

            Console::logDebug("{} {}", rotation, deltaY);
            
        }

        void onDoubleClick(UserInterface::MouseButton button, Float64 x, Float64 y) override {

            if(button == UserInterface::MouseButton::Left) {
                rotation = 0.0;
            }
            
        }

        void initialize() override {

            setScale({ 150, 150 });
            setColor(UserInterface::Colors::white);
            
        }

        void draw(SkCanvas* canvas, Float64 deltaTime) override {

            SkPaint paint;
            const SkVector pos = getAbsolutePosition();
            const SkVector size = getScale();
            const SkRect rect = SkRect::MakeXYWH(pos.x(), pos.y(), size.x(), size.y());
            const SkVector mid = getAbsoluteCenteredPosition();
            paint.setColor(getColor().toSkColor());
            canvas->save();
            canvas->rotate(f32(rotation), mid.fX, mid.fY);
            canvas->drawRect(rect, paint);
            canvas->restore();
            
        }

    private:

        Float64 rotation = 0.0;
        
    };

    void Application::run() {

        fassertf(!instance, "Tried to run multiple Applications at the same time");

        instance = this;

        this->console = UniquePointer<Console>::make();

        console->init();

        setTerminationCallback(&Application::onApplicationTerminate);

        registerCommands();

        glfwSetErrorCallback(&Application::onError);

        fassertf(glfwInit(), "Failed to initialize GLFW");

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

        this->mainWindow = glfwCreateWindow(1280, 720, "Application", nullptr, nullptr);

        fassertf(this->mainWindow, "Failed to create window");

        glfwMakeContextCurrent(mainWindow);

        // Disable VSync
        glfwSwapInterval(0);

        glfwSetKeyCallback(mainWindow, &Application::inputCallback);
        glfwSetMouseButtonCallback(mainWindow, &Application::mouseCallback);
        glfwSetScrollCallback(mainWindow, &Application::scrollCallback);
        glfwSetCursorPosCallback(mainWindow, &Application::cursorCallback);
        
        Int w, h;
        glfwGetFramebufferSize(mainWindow, &w, &h);

        // todo: Find the framebuffer ID with OpenGL.
        GLint framebufferId = 0;
        
        this->masterView = UserInterface::MasterView::makeGL(w, h, {framebufferId, 4, 8});

        initializeAudio();

        this->cursorManager = SharedPointer<UserInterface::CursorManager>::make();
        cursorManager->setCompound(masterView);
        
        masterView->addChild<FrameInfo>();
        auto we = masterView->addChild<RotaryKnob>();
        we->setPosition({500, 500});
        this->shouldRun = true;

        Console::logStatus("Created new Flux Application.");

        update();

        Console::logStatus("Exiting Application.");

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

        this->audioDevice = SharedPointer<MyAudioDevice>::make();

        audioDevice->initialize(44100, 128);

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

    void Application::onApplicationTerminate() { Console::logFatal("Application received termination signal"); }

    void Application::registerCommands() {

        const auto stopNode = CommandNode::make("Application.Stop");

        stopNode->setNodeDescription("Exits the application.");
        stopNode->addExecutable([](auto) { instance->stop(); });

        Console::registerCommand(stopNode);

    }

}
