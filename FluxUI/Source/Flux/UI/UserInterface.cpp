#include <Flux/UI/UserInterface.h>

#define SK_GL

#include <GLFW/glfw3.h>
#include <skia/gpu/GrBackendSurface.h>
#include <skia/gpu/GrDirectContext.h>
#include <skia/gpu/gl/GrGLInterface.h>
#include <skia/core/SkCanvas.h>
#include <skia/core/SkSurface.h>
#include <skia/core/SkColorSpace.h>

namespace Flux::UserInterface {

    void MasterView::draw() {

        fassert(surface != nullptr);

        SkPaint paint;
        paint.setColor(SK_ColorGRAY);
        canvas->drawPaint(paint);
        context->flush();
        
    }

    SharedPointer<MasterView> MasterView::makeGL(Int width, Int height, OpenGLParams const& params) {

        auto view = SharedPointer<MasterView>::make();

        const auto interface = GrGLMakeNativeInterface();
        view->context = GrDirectContext::MakeGL(interface).release();

        fassertf(view->context, "Failed to create Skia GL context.");
        
        GrGLFramebufferInfo framebufferInfo;

        framebufferInfo.fFBOID = params.framebufferId;
        framebufferInfo.fFormat = GL_RGBA8;
        
        SkColorType colorType = kRGBA_8888_SkColorType;
        
        GrBackendRenderTarget backendRenderTarget(width, height, params.sampleCount, params.stencilBits, framebufferInfo);

        view->surface = SkSurface::MakeFromBackendRenderTarget(view->context, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr).release();

        fassertf(view->surface, "Failed to create Skia Surface.");

        view->canvas = view->surface->getCanvas();

        fassertf(view->canvas, "Failed to get Skia Canvas.");
        
        return view;
        
    }
    
}
