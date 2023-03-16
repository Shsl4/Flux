#pragma once

#include <GLFW/glfw3.h>
#include <Flux/UI/CursorManager.h>
#include <Flux/UI/UserInterface.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/gpu/GrDirectContext.h>

#define GL_FRAMEBUFFER_BINDING 0x8CA6

namespace Flux {

    using namespace UI;

    class FrameInfo : public UI::Component {

    public:

        FrameInfo(Point const& p, Point const& s) : Component(p, s) {

            setColor(Color::fromHex(0x60606060));

        }

        void draw(SkCanvas* canvas, Float64 deltaTime) override {

            const Float64 currentTime = glfwGetTime();
            ++frameCount;

            if(currentTime - lastTime >= 1.0) {

                lastFrameTime = (1000.0 / static_cast<Float64>(frameCount));
                frameCount = 0;
                lastTime += 1.0;

            }

            SkPaint paint;

            paint.setColor(color().skColor());

            const Point pos = globalTransform().position;
            const SkRect rect = SkRect::MakeXYWH(pos.x, pos.y, size().x, size().y);

            canvas->drawRoundRect(rect, 10, 10, paint);

            paint.setColor(SK_ColorWHITE);

            String text = String::format("Draw: {} FPS ({}ms)", 1000.0 * (1.0 / lastFrameTime), lastFrameTime);
            SkFont font;
            font.setSize(12.0);

            canvas->drawSimpleText(text.begin().get(), text.size(), SkTextEncoding::kUTF8, pos.x + 10.0f,
                                   pos.y + 20.0f, font, paint);


        }

    private:

        Float64 lastTime = 0.0;
        UInt frameCount = 0;
        Float64 lastFrameTime = 0.0;

    };

    class Window {

    public:

        virtual void draw(Float64 const& deltaTime) = 0;

        void swapBuffers() const;

        virtual ~Window();

        NODISCARD UI::Component* mainView() const { return this->rootComponent; }

    protected:

        friend class Application;

        Window() = default;

        Shared<UI::CursorManager> cursorManager = nullptr;
        UI::Component* rootComponent = nullptr;
        UI::MouseButton lastButtonPressed = UI::MouseButton::Size;
        GLFWwindow* handle = nullptr;
        Float64 lastClickTime = 0.0;

        static void inputCallback(GLFWwindow* window, Int key, Int scancode, Int action, Int mods);

        static void mouseCallback(GLFWwindow* window, Int button, Int action, Int mods);

        static void scrollCallback(GLFWwindow* window, Float64 xOffset, Float64 yOffset);
        
        static void cursorCallback(GLFWwindow* window, Float64 xPos, Float64 yPos);
        
        static void closeCallback(GLFWwindow* window);
        
        static Window* windowFromHandle(GLFWwindow* handle);

    };

}
