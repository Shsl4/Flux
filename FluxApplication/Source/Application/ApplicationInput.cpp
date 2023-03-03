#include <Application/Application.h>

namespace Flux {
    
    void Application::inputCallback(GLFWwindow* window, Int key, Int scancode, Int action, Int mods) {

        // Ignore key repeat when held
        if(action == GLFW_REPEAT) return;

        instance->audioDevice->simulateMessage(key, action);
        
    }

    void Application::mouseCallback(GLFWwindow* window, Int button, Int action, Int mods) {
        
        if(action) {
            
            instance->cursorManager->buttonDown(static_cast<UI::MouseButton>(button));
            
        }
        else {

            const auto mouseButton = static_cast<UI::MouseButton>(button);
            const Float64 newTime = glfwGetTime();

            if(newTime - instance->lastClickTime < 0.250 && mouseButton == instance->lastButtonPressed) {
                instance->cursorManager->doubleClick(mouseButton);
            }

            instance->lastClickTime = newTime;
            instance->lastButtonPressed = mouseButton;
            
            instance->cursorManager->buttonUp(static_cast<UI::MouseButton>(button));
            
        }
        
    }

    void Application::scrollCallback(GLFWwindow* window, Float64 xOffset, Float64 yOffset) {

        instance->cursorManager->scroll(xOffset, yOffset);
        
    }

    void Application::cursorCallback(GLFWwindow* window, Float64 xPos, Float64 yPos) {

        instance->cursorManager->cursorMoved(xPos, yPos);
        
    }

}
