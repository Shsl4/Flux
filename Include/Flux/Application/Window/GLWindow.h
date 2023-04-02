#pragma once

#include <Application/Window/Window.h>

namespace Flux {
    
    
    class GLWindow : public Window {

    public:

        GLWindow(const String &title, Int windowWidth, Int windowHeight, Component* rootComponent = nullptr);
        
        void draw(Float64 const& deltaTime) override;

        void resized(Int width, Int height) override;

        NODISCARD CursorManager* cursorManager() const override { return this->manager; }

        NODISCARD Component* mainComponent() const override { return this->component; }

    private:

        Component* component = nullptr;
        CursorManager* manager = nullptr;
        SkCanvas* canvas = nullptr;
        
        sk_sp<SkSurface> surface = nullptr;
        GrDirectContext* context = nullptr;
        Point windowSize = {};
        
        Float32 dpiScale = 1.0f;

        void setupCanvas(Int width, Int height);

    };

    
}
