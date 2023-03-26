#include <Application/Application.h>
#include <Application/Window/Window.h>

namespace Flux {
    
    void Window::inputCallback(GLFWwindow* window, Int key, Int scancode, Int action, Int mods) {

        // Ignore key repeat when held
        if(action == GLFW_REPEAT) return;

        if(action == GLFW_PRESS) {
            windowFromHandle(window)->cursorManager()->handleKeyDown({ static_cast<char>(key), mods });
        }

        if(action == GLFW_RELEASE) {
            windowFromHandle(window)->cursorManager()->handleKeyUp({ static_cast<char>(key), mods });
        }

        Application::instance()->audioDevice->simulateMessage(key, action);
        
    }

    void Window::mouseCallback(GLFWwindow* window, Int button, Int action, Int mods) {

        Window* appWindow = windowFromHandle(window);

        if(action) {

            appWindow->cursorManager()->handleButtonDown(static_cast<MouseButton>(button));
            
        }
        else {

            const auto mouseButton = static_cast<MouseButton>(button);
            const Float64 newTime = glfwGetTime();

            if(newTime - appWindow->lastClickTime < 0.250 && mouseButton == appWindow->lastButtonPressed) {
                appWindow->cursorManager()->handleDoubleClick(mouseButton);
            }

            appWindow->lastClickTime = newTime;
            appWindow->lastButtonPressed = mouseButton;

            appWindow->cursorManager()->handleButtonUp(static_cast<MouseButton>(button));
            
        }
        
    }

    void Window::scrollCallback(GLFWwindow* window, Float64 xOffset, Float64 yOffset) {

        windowFromHandle(window)->cursorManager()->handleScroll(xOffset, yOffset);
        
    }

    void Window::cursorCallback(GLFWwindow* window, Float64 xPos, Float64 yPos) {

        windowFromHandle(window)->cursorManager()->handleCursorMoved(xPos, yPos);
        
    }

    void Window::closeCallback(GLFWwindow* window) {

        Factory::destroyWindow(windowFromHandle(window));

    }


    Window *Window::windowFromHandle(GLFWwindow *handle) {
        for(const auto& w : Factory::windows()){
            if (w->handle == handle){
                return w;
        
            }
        }
        return nullptr;
    }

    void Window::resizeCallback(GLFWwindow *window, Int width, Int height) {

        windowFromHandle(window)->resized(width, height);

    }

}
