#include <Application/Window/Window.h>
#include <Application/Window/GLWindow.h>
#include <Application/Window/MetalWindow.h>
#include <skia/include/core/SkFont.h>

namespace Flux {
    
    Window::~Window() {
        glfwDestroyWindow(handle);
    }
    
}
