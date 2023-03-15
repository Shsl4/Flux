#include <Application/Window.h>
#include <skia/include/core/SkFont.h>

namespace Flux {
    
    class OpenGLWindow : public Window {

    public:

        OpenGLWindow(const String &title, Int windowWidth, Int windowHeight);
        
        virtual void draw(Float64 const& deltaTime) override{
            Window::draw(deltaTime);
        }

    };

    OpenGLWindow::OpenGLWindow(const String &title, Int windowWidth, Int windowHeight) {
        
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        
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

        this->handle = glfwCreateWindow(windowWidth, windowHeight, title.begin().get(), nullptr, nullptr);

        if (!this->handle) {
            throw Exceptions::Exception("Failed to create window handle!");
        }

        glfwMakeContextCurrent(this->handle);

        glfwSwapInterval(1);

        glfwSetKeyCallback(this->handle, &Window::inputCallback);
        glfwSetMouseButtonCallback(this->handle, &Window::mouseCallback);
        glfwSetScrollCallback(this->handle, &Window::scrollCallback);
        glfwSetCursorPosCallback(this->handle, &Window::cursorCallback);

        Int framebufferWidth;
        Int framebufferHeight;

        glfwGetFramebufferSize(this->handle, &framebufferWidth, &framebufferHeight);

        Int32 framebufferId = 0;

        const auto glGetIntegerv = reinterpret_cast<void (*)(UInt32, Int32 *)>(glfwGetProcAddress("glGetIntegerv"));

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebufferId);

        const OpenGLParams params = {framebufferId, 4, 8, f32(framebufferWidth) / f32(windowWidth),
                                     f32(framebufferHeight) / f32(windowHeight)};

        this->rootComponent = MasterView::makeGL(framebufferWidth, framebufferHeight, params);
        this->cursorManager = Shared<CursorManager>::make();
        this->cursorManager->setComponent(this->rootComponent);

        constexpr Float32 fw = 300.0f;
        constexpr Float32 fh = 30.0f;

        Point p = { f32(windowWidth) - fw - 10.0f, f32(windowHeight) - fh - 10.0f};

        this->rootComponent->addChild(Component::Factory::create<FrameInfo>(p, Point(fw, fh)));

    }

    Window* Window::Factory::createWindow(RenderBackend backEnd, const String &name, Int width, Int height) {

        Window* window;

        switch (backEnd) {

            case RenderBackend::OpenGL:
                window = Allocator<OpenGLWindow>::construct(name, width, height);
                break;
            case RenderBackend::Metal:
                window = Allocator<MetalWindow>::construct(name, width, height);
                break;
            case RenderBackend::Vulkan:
            case RenderBackend::DirectX:
            default:
                throw Exceptions::Exception("Unsupported backend.");
        }

        activeWindows += window;

        return window;

    }
    
}
