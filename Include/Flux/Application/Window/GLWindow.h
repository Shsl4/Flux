#pragma once

#include <Application/Window/Window.h>

namespace Flux {
    
    
    class GLWindow : public Window {

    public:

        GLWindow(const String &title, Int windowWidth, Int windowHeight);
        
        void draw(Float64 const& deltaTime) override;

        void resized(Int width, Int height) override;

    private:
        
        sk_sp<SkSurface> surface = nullptr;
        SkCanvas* canvas = nullptr;
        GrDirectContext* context = nullptr;
        Float32 dpiScale = 1.0f;

        void setupCanvas(Int width, Int height);

    };

    
}
