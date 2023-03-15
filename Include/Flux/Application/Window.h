#pragma once

#include <GLFW/glfw3.h>
#include <Flux/UI/CursorManager.h>
#include <Flux/UI/UserInterface.h>
#include <skia/include/core/SkFont.h>

#define GL_FRAMEBUFFER_BINDING 0x8CA6

namespace Flux {

    using namespace UI;

    enum class RenderBackend{

        OpenGL = 0,
        Vulkan,
        Metal,
        DirectX

    };


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

        class Factory final {

        public:

            static Window* createWindow(RenderBackend backEnd, String const& name, Int width, Int height);

            NODISCARD FORCEINLINE static SmartArray<Window> const& windows() { return activeWindows; };

        private:

            friend class Window;
            friend class Application;

            static inline SmartArray<Window> activeWindows = {};

        };

        virtual void draw(Float64 const& deltaTime);

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
        
        static void charCallback(GLFWwindow* window, UInt codepoint);

        static Window* windowFromHandle(GLFWwindow* handle);

    };

    class MetalWindow : public Window {

    public:

        MetalWindow(const String &title, Int windowWidth, Int windowHeight);

        void draw(const Float64 &deltaTime) override;
        
    private:
        
        void* mtkView = nullptr;
        void* mtlLayer = nullptr;
        void* mtlQueue = nullptr;
        const void* mtlDrawable = nullptr;
        
        sk_sp<SkSurface> surface = nullptr;
        SkCanvas* canvas = nullptr;
        sk_sp<GrDirectContext> context = nullptr;
        Float32 dpiScale = 1.0f;

    };

}
