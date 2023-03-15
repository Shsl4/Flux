#include <Application/Window.h>
#include <Application/OpenGLWindow.h>
#include <skia/include/core/SkFont.h>

namespace Flux {

    Window* Window::Factory::createWindow(RenderBackend backEnd, const String &name, Int width, Int height) {

        Window* window;

        switch (backEnd) {

            case RenderBackend::OpenGL:
                window = Allocator<OpenGLWindow>::construct(name, width, height);
                break;
            case RenderBackend::Metal:
#ifdef __APPLE__
                window = Allocator<MetalWindow>::construct(name, width, height);
#else
                throw Exceptions::Exception("Unsupported backend.");
#endif
                break;
            case RenderBackend::Vulkan:
            case RenderBackend::DirectX:
            default:
                throw Exceptions::Exception("Unsupported backend.");
        }

        activeWindows += window;

        return window;

    }
    
    void Window::swapBuffers() const {
        glfwSwapBuffers(this->handle);
    }

    Window::~Window() {
        glfwDestroyWindow(handle);
    }
    
}
