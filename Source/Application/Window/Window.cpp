#include <Application/Window/Window.h>
#include <Application/Window/GLWindow.h>
#include <Application/Window/MetalWindow.h>

namespace Flux {
    
    Window::~Window() {
        glfwDestroyWindow(handle);
    }
    
}
