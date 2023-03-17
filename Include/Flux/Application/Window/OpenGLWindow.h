#pragma once

#include <Application/Window/Window.h>

namespace Flux {
    
    
    class OpenGLWindow : public Window {

    public:

        OpenGLWindow(const String &title, Int windowWidth, Int windowHeight);
        
        void draw(Float64 const& deltaTime) override;
        
    private:
        
        sk_sp<SkSurface> surface = nullptr;
        SkCanvas* canvas = nullptr;
        GrDirectContext* context = nullptr;
        Float32 dpiScale = 1.0f;

    };

    
}
