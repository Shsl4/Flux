#include <Application/Application.h>

namespace Flux {
    
    void Application::inputCallback(GLFWwindow* window, Int key, Int scancode, Int action, Int mods) {

        // Ignore key repeat when held
        if(action == GLFW_REPEAT) return;
        
        instance->audioDevice->simulateMidiMessage(key, action);
        
    }

    void Application::mouseCallback(GLFWwindow* window, Int button, Int action, Int mods) {
        
        if(action) {
            
            instance->cursorManager->onButtonDown(static_cast<UserInterface::MouseButton>(button));
            
        }
        else {

            const auto mouseButton = static_cast<UserInterface::MouseButton>(button);
            const Float64 newTime = glfwGetTime();

            if(newTime - instance->lastClickTime < 0.250 && mouseButton == instance->lastButtonPressed) {
                instance->cursorManager->onDoubleClick(mouseButton);
            }

            instance->lastClickTime = newTime;
            instance->lastButtonPressed = mouseButton;
            
            instance->cursorManager->onButtonUp(static_cast<UserInterface::MouseButton>(button));
            
        }
        
    }

    void Application::scrollCallback(GLFWwindow* window, Float64 xOffset, Float64 yOffset) {

        instance->cursorManager->onScroll(xOffset, yOffset);
        
    }

    void Application::cursorCallback(GLFWwindow* window, Float64 xPos, Float64 yPos) {

        instance->cursorManager->onCursorMoved(xPos, yPos);
        
    }

}
