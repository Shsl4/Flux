#pragma once

#include <Flux/UI/Component.h>

class GrDirectContext;

namespace Flux::UI {
    
    struct OpenGLParams {
        
        OpenGLParams(const Int framebufferId, const Int sampleCount, const Int stencilBits, const Float32 xDpiScale,
            const Float32 yDpiScale) : framebufferId(framebufferId), sampleCount(sampleCount), stencilBits(stencilBits),
                xDpiScale(xDpiScale), yDpiScale(yDpiScale){}

        Int framebufferId = 0;
        Int sampleCount = 0;
        Int stencilBits = 0;
        Float32 xDpiScale = 1.0;
        Float32 yDpiScale = 1.0;
        
    };
    
    class MasterView : public Component {
    
    public:

        MasterView();

        void draw(SkCanvas* canvas, Float64 deltaTime) override;
        
        static MasterView* makeGL(Int width, Int height, OpenGLParams const& params);
    
    private:
        
        SkSurface* surface = nullptr;
        SkCanvas* canvas = nullptr;
        GrDirectContext* context = nullptr;
        Float32 xDpiScale = 1.0;
        Float32 yDpiScale = 1.0;
        
    };
    
}
