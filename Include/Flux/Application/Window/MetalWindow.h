#pragma once

#include <Application/Window/Window.h>

namespace Flux {
    
    class MetalWindow : public Window {

    public:

        MetalWindow(const String &title, Int windowWidth, Int windowHeight);

        void draw(const Float64 &deltaTime) override;
    
    private:
    
        void* mtkView = nullptr;
        void* mtlQueue = nullptr;
        const void* mtlDrawable = nullptr;
        sk_sp<GrDirectContext> context = nullptr;
        Float32 dpiScale = 1.0f;

    };
    
}
