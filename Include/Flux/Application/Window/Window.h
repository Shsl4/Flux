#pragma once

#include <GLFW/glfw3.h>
#include <Flux/UI/CursorManager.h>
#include <Flux/UI/Component.h>

#define GL_FRAMEBUFFER_BINDING 0x8CA6

namespace Flux {

    class Window {

    public:

        virtual void draw(Float64 const& deltaTime) = 0;

        virtual void resized(Int width, Int height) = 0;

        NODISCARD virtual CursorManager* cursorManager() const = 0;

        NODISCARD virtual Component* mainComponent() const = 0;
        
        virtual ~Window();
    
    protected:

        friend class Application;

        Window() = default;
        
        MouseButton lastButtonPressed = MouseButton::Size;
        GLFWwindow* handle = nullptr;
        Float64 lastClickTime = 0.0;

        static void inputCallback(GLFWwindow* window, Int key, Int scancode, Int action, Int mods);

        static void mouseCallback(GLFWwindow* window, Int button, Int action, Int mods);

        static void scrollCallback(GLFWwindow* window, Float64 xOffset, Float64 yOffset);
        
        static void cursorCallback(GLFWwindow* window, Float64 xPos, Float64 yPos);
        
        static void closeCallback(GLFWwindow* window);

        static void resizeCallback(GLFWwindow* window, Int width, Int height);

        static Window* windowFromHandle(GLFWwindow* handle);

    };

}
