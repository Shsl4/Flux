#pragma once

#define SK_GL
#define SK_METAL
#define SK_GANESH

#include <skia/include/gpu/gl/GrGLInterface.h>
#include <skia/include/core/SkColorSpace.h>
#include <skia/include/gpu/GrBackendSurface.h>
#include <skia/include/gpu/GrDirectContext.h>
#include <skia/include/gpu/mtl/GrMtlBackendContext.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkRRect.h>
#include <skia/include/gpu/GrBackendSurface.h>

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
        static MasterView* makeMetal(Int width, Int height, Float32 dpiScale, void *device, void *commandQueue, void* v, Int sampleCount);
        static MasterView* makeMetalFromLayer(Int width, Int height, Float32 dpiScale, void *device, void *commandQueue, void* layer, GrMTLHandle* drawable, Int sampleCount);

    private:
        
        sk_sp<SkSurface> surface = nullptr;
        SkCanvas* canvas = nullptr;
        GrDirectContext* context = nullptr;
        const void* current_drawable = nullptr;
        Float32 xDpiScale = 1.0;
        Float32 yDpiScale = 1.0;
        
    };
    
}
