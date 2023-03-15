#pragma once

#include <Application/Window.h>

namespace Flux {
    
    
    class OpenGLWindow : public Window {

    public:

        OpenGLWindow(const String &title, Int windowWidth, Int windowHeight);
        
        void draw(Float64 const& deltaTime) override;
        
        void setupCanvas() {
            
            const auto itf = GrGLMakeNativeInterface();
            this->context = GrDirectContext::MakeGL(itf).release();

            if(!this->context) throw Exceptions::Exception("Failed to create Skia GL context.");

            GrGLFramebufferInfo framebufferInfo;

            framebufferInfo.fFBOID = 0;
            
            // 0x8058 = GL_RGBA8
            framebufferInfo.fFormat = 0x8058;

            constexpr SkColorType colorType = kRGBA_8888_SkColorType;

            const GrBackendRenderTarget backendRenderTarget(1280, 720, 4, 8, framebufferInfo);
                    
            this->surface = SkSurface::MakeFromBackendRenderTarget(this->context, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr);

            if(!this->surface) throw Exceptions::Exception("Failed to create Skia Surface.");

            this->canvas = this->surface->getCanvas();

            if(!this->surface) throw Exceptions::Exception("Failed to get Skia Canvas.");
            
        }
        
    private:
        
        sk_sp<SkSurface> surface = nullptr;
        SkCanvas* canvas = nullptr;
        GrDirectContext* context = nullptr;

    };

    
}
