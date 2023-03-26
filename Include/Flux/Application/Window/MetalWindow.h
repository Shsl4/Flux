#pragma once

#include <Application/Window/Window.h>

namespace Flux {
    
    class MetalWindow : public Window {

    public:

        MetalWindow(const String &title, Int windowWidth, Int windowHeight, Component* rootComponent = nullptr);

        void draw(const Float64 &deltaTime) override;

        void resized(Int width, Int height) override;

        NODISCARD CursorManager* cursorManager() const override { return this->manager; }

        NODISCARD Component* mainComponent() const override { return this->component; }

    private:

        Component* component = nullptr;
        CursorManager* manager = nullptr;
        
        void* mtkView = nullptr;
        void* mtlQueue = nullptr;
        sk_sp<GrDirectContext> context = nullptr;
        Float32 dpiScale = 1.0f;

    };
    
}
