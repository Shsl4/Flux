#include "Application.h"

#undef APIENTRY
#include <Windows.h>
#include <skia/core/SkDrawable.h>
#include <skia/core/SkPath.h>
#include <skia/core/SkRRect.h>

#include "DragComponent.h"
#include "Node.h"
#include "Socket.h"

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

        GLint drawFboId = 0;

        HMODULE libGL = LoadLibraryW(L"opengl32.dll");
        void (*e)(GLenum, Int*) = (void(*)(GLenum, Int*))GetProcAddress(libGL, "glGetIntegerv");
        e(0x8CA6, &drawFboId);
        FreeLibrary(libGL);

        initializeAudio();

        this->masterView = UserInterface::MasterView::makeGL(1280, 720, {drawFboId, 4, 8});

        this->cursorManager = SharedPointer<UserInterface::CursorManager>::make();

        cursorManager->setCompound(masterView.raw());

        masterView->addChild<FrameInfo>();

        auto nodeA = masterView->addChild<Node>();

        nodeA->setPosition({ 400, 300 });

        
        auto nodeB = masterView->addChild<Node>();

        nodeB->setPosition({ 700, 200 });
        
        Socket* inputSocketA = nodeA->getChild(0)->cast<Socket>();
        Socket* outputSocketA = nodeA->getChild(1)->cast<Socket>();
        Socket* inputSocketB = nodeB->getChild(0)->cast<Socket>();
        Socket* outputSocketB = nodeB->getChild(1)->cast<Socket>();

        inputSocketA->link(outputSocketB);
        inputSocketB->link(outputSocketA);
        
/*
        auto component = masterView->addChild<Socket>();
        component->setPosition({ 100, 100 });
        component->setScale({ 25, 25 });
        component = masterView->addChild<Socket>();
        component->setFlow(Socket::Flow::Input);
        component->setPosition({ 500, 100 });
        component->setScale({ 25, 25 });
*/
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

        audioDevice->initialize(44100, 256);

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
