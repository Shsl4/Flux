#include <Application/Application.h>
#include "Application/Window.h"


namespace Flux {
    
    void Window::inputCallback(GLFWwindow* window, Int key, Int scancode, Int action, Int mods) {

        // Ignore key repeat when held
        if(action == GLFW_REPEAT) return;

        // Application::instance->audioDevice->simulateMessage(key, action);
        
    }
    
    void Window::charCallback(GLFWwindow* window, UInt codepoint){
                
    }

    void Window::mouseCallback(GLFWwindow* window, Int button, Int action, Int mods) {

        Window* appWindow = windowFromHandle(window);

        if(action) {

            appWindow->cursorManager->buttonDown(static_cast<UI::MouseButton>(button));
            
        }
        else {

            const auto mouseButton = static_cast<UI::MouseButton>(button);
            const Float64 newTime = glfwGetTime();

            if(newTime - appWindow->lastClickTime < 0.250 && mouseButton == appWindow->lastButtonPressed) {
                appWindow->cursorManager->doubleClick(mouseButton);
            }

            appWindow->lastClickTime = newTime;
            appWindow->lastButtonPressed = mouseButton;

            appWindow->cursorManager->buttonUp(static_cast<UI::MouseButton>(button));
            
        }
        
    }

    void Window::scrollCallback(GLFWwindow* window, Float64 xOffset, Float64 yOffset) {

        windowFromHandle(window)->cursorManager->scroll(xOffset, yOffset);
        
    }

    void Window::cursorCallback(GLFWwindow* window, Float64 xPos, Float64 yPos) {

        windowFromHandle(window)->cursorManager->cursorMoved(xPos, yPos);
        
    }

    Window *Window::windowFromHandle(GLFWwindow *handle) {
        for(const auto& w : Window::Factory::activeWindows){
            if (w->handle == handle){
                return w;
        
            }
        }
        return nullptr;
    }

    void Window::swapBuffers() const {
        glfwSwapBuffers(this->handle);
    }

    void Window::draw(const Float64 &deltaTime) {

        glfwMakeContextCurrent(this->handle);

        this->rootComponent->draw(nullptr, deltaTime);

        swapBuffers();

    }

    Window::~Window() {
        glfwDestroyWindow(handle);
    }

}
