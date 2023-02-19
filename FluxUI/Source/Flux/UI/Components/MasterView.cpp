#include <Components/MasterView.h>

#ifndef SK_GL
#define SK_GL
#endif

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

    void MasterView::draw(SkCanvas*, Float64 deltaTime) {

        assert(canvas && surface);
        
        canvas->save();
        canvas->scale(xDpiScale, yDpiScale);
        
        Component::draw(canvas, deltaTime);
        Compound::draw(canvas, deltaTime);
        
        context->flush();
        canvas->restore();
      
    }
    
    void MasterView::initialize() {
        
        Compound::initialize();

        this->setColor(LinearColor::fromHex(0x151515ff));
        
    }

    Shared<MasterView> MasterView::makeGL(Int width, Int height, OpenGLParams const& params) {

        auto view = Shared<MasterView>::make();

        const auto itf = GrGLMakeNativeInterface();
        view->context = GrDirectContext::MakeGL(itf).release();

        if(!view->context) throw Exceptions::Exception("Failed to create Skia GL context.");

        GrGLFramebufferInfo framebufferInfo;

        framebufferInfo.fFBOID = params.framebufferId;
        
        // 0x8058 = GL_RGBA8
        framebufferInfo.fFormat = 0x8058;
        
        SkColorType colorType = kRGBA_8888_SkColorType;
        
        GrBackendRenderTarget backendRenderTarget(width, height, params.sampleCount, params.stencilBits, framebufferInfo);

        view->surface = SkSurface::MakeFromBackendRenderTarget(view->context, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr).release();

        if(!view->surface) throw Exceptions::Exception("Failed to create Skia Surface.");

        view->canvas = view->surface->getCanvas();

        if(!view->surface) throw Exceptions::Exception("Failed to get Skia Canvas.");

        auto w = static_cast<Float32>(view->canvas->getBaseLayerSize().fWidth);
        auto h = static_cast<Float32>(view->canvas->getBaseLayerSize().fHeight);
        
        view->xDpiScale = params.xDpiScale;
        view->yDpiScale = params.yDpiScale;

        view->setScale({w, h});
        view->initialize();
        
        return view;
        
    }
    
}
