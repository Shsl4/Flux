#include <Components/MasterView.h>

#define SK_GL

#include <skia/core/SkCanvas.h>
#include <skia/gpu/GrBackendSurface.h>
#include <skia/gpu/GrDirectContext.h>
#include <skia/gpu/gl/GrGLInterface.h>
#include <skia/core/SkCanvas.h>
#include <skia/core/SkSurface.h>
#include <skia/core/SkFont.h>
#include <skia/core/SkRRect.h>
#include <skia/core/SkColorSpace.h>
#include <skia/gpu/GrBackendSurface.h>

namespace Flux::UserInterface {

    void MasterView::draw(SkCanvas*) {

        fassert(surface != nullptr);

        Compound::draw(canvas);

        context->flush();
        
    }

    void MasterView::initialize() {
        
        Compound::initialize();

        this->setColor(LinearColor::fromHex(0x303030ff));
        
    }

    SharedPointer<MasterView> MasterView::makeGL(Int width, Int height, OpenGLParams const& params) {

        auto view = SharedPointer<MasterView>::make();

        const auto interface = GrGLMakeNativeInterface();
        view->context = GrDirectContext::MakeGL(interface).release();

        fassertf(view->context, "Failed to create Skia GL context.");
        
        GrGLFramebufferInfo framebufferInfo;

        framebufferInfo.fFBOID = params.framebufferId;
        // 0x8058 = GL_RGBA8
        framebufferInfo.fFormat = 0x8058;
        
        SkColorType colorType = kRGBA_8888_SkColorType;
        
        GrBackendRenderTarget backendRenderTarget(width, height, params.sampleCount, params.stencilBits, framebufferInfo);

        view->surface = SkSurface::MakeFromBackendRenderTarget(view->context, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr).release();

        fassertf(view->surface, "Failed to create Skia Surface.");

        view->canvas = view->surface->getCanvas();

        fassertf(view->canvas, "Failed to get Skia Canvas.");

        auto w = static_cast<Float32>(view->canvas->getBaseLayerSize().fWidth);
        auto h = static_cast<Float32>(view->canvas->getBaseLayerSize().fHeight);
        
        view->setScale({w, h});
        view->initialize();
        
        return view;
        
    }
    
}
