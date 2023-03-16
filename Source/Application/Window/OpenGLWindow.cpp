#include <Application/Window/OpenGLWindow.h>
#include <Flux/Factory.h>

#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/gpu/GrDirectContext.h>
#include <skia/include/gpu/gl/GrGLInterface.h>
#include <skia/include/gpu/gl/GrGLTypes.h>

namespace Flux {
    
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
        glfwSetWindowCloseCallback(this->handle, &Window::closeCallback);

        Int framebufferWidth;
        Int framebufferHeight;

        glfwGetFramebufferSize(this->handle, &framebufferWidth, &framebufferHeight);

        Int32 framebufferId = 0;

        const auto glGetIntegerv = reinterpret_cast<void (*)(UInt32, Int32 *)>(glfwGetProcAddress("glGetIntegerv"));

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebufferId);
        
        this->dpiScale = f32(framebufferWidth) / f32(windowWidth);
        
        const auto itf = GrGLMakeNativeInterface();
        
        this->context = GrDirectContext::MakeGL(itf).release();

        if(!this->context) throw Exceptions::Exception("Failed to create Skia GL context.");

        GrGLFramebufferInfo framebufferInfo;

        framebufferInfo.fFBOID = framebufferId;
        
        // 0x8058 = GL_RGBA8
        framebufferInfo.fFormat = 0x8058;

        constexpr SkColorType colorType = kRGBA_8888_SkColorType;

        const GrBackendRenderTarget backendRenderTarget(windowWidth * dpiScale, windowHeight * dpiScale, 4, 8, framebufferInfo);
                
        this->surface = SkSurface::MakeFromBackendRenderTarget(this->context, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr);

        if(!this->surface) throw Exceptions::Exception("Failed to create Skia Surface.");

        this->canvas = this->surface->getCanvas();

        if(!this->surface) throw Exceptions::Exception("Failed to get Skia Canvas.");
        
        this->rootComponent = Flux::Factory::create<Component>(Point(0, 0), Point(windowWidth, windowHeight));
        this->cursorManager = Shared<CursorManager>::make();
        
        this->cursorManager->setComponent(this->rootComponent);

        constexpr Float32 fw = 300.0f;
        constexpr Float32 fh = 30.0f;

        Point p = { f32(windowWidth) - fw - 10.0f, f32(windowHeight) - fh - 10.0f };

        this->rootComponent->addChild(Flux::Factory::create<FrameInfo>(p, Point(fw, fh)));
        
    }

    void OpenGLWindow::draw(const Float64 &deltaTime) {

        glfwMakeContextCurrent(this->handle);

        canvas->save();
        canvas->scale(dpiScale, dpiScale);
        
        this->rootComponent->draw(this->canvas, deltaTime);

        context->flush();
        canvas->restore();
        
        glfwSwapBuffers(this->handle);

    }

    
}
