#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <MetalKit/MetalKit.h>
#include <Application/Window.h>

namespace Flux {

    MetalWindow::MetalWindow(const String &title, Int windowWidth, Int windowHeight) {

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

        this->handle = glfwCreateWindow(windowWidth, windowHeight, title.begin().get(), nullptr, nullptr);

        if (!this->handle) {
            throw Exceptions::Exception("Failed to create window handle!");
        }

        glfwSetKeyCallback(this->handle, &Window::inputCallback);
        glfwSetMouseButtonCallback(this->handle, &Window::mouseCallback);
        glfwSetScrollCallback(this->handle, &Window::scrollCallback);
        glfwSetCursorPosCallback(this->handle, &Window::cursorCallback);
        glfwSetCharCallback(this->handle, &Window::charCallback);
        
        NSWindow* rawWindow = glfwGetCocoaWindow(this->handle);
        
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        id<MTLCommandQueue> queue = [device newCommandQueue];
        
        MTKView* view = [[MTKView alloc] initWithFrame: { 0, 0, f32(windowWidth), f32(windowHeight) } device:device];
        [[rawWindow contentView] addSubview:view];
        
        this->mtkView = view;
        this->mtlQueue = queue;
        /*
        CAMetalLayer* layer = [CAMetalLayer layer];
        layer.device = device;
        layer.opaque = YES;
        this->mtlLayer = layer;
        
        rawWindow.contentView.layer = layer;
        rawWindow.contentView.wantsLayer = YES;
        
         */
        
        this->dpiScale = [rawWindow backingScaleFactor];
        
       // this->mtlDrawable = [layer nextDrawable];
        
        this->rootComponent = Component::Factory::create<Component>(Point(0, 0), Point(windowWidth * dpiScale, windowHeight * dpiScale));
        
        //MasterView::makeMetal(windowWidth, windowHeight, f32([rawWindow backingScaleFactor]),
          //                                          device, queue, view, i32([view sampleCount]));
        //this->rootComponent = MasterView::makeMetalFromLayer(windowWidth, windowHeight, f32([rawWindow backingScaleFactor]), device, queue, layer, &this->mtlDrawable, 1);
        
        this->context = GrDirectContext::MakeMetal(device, queue);

        if(!this->context) throw Exceptions::Exception("Failed to create Skia metal context.");

        constexpr SkColorType colorType = kBGRA_8888_SkColorType;
        Float32 sampleCount = [view sampleCount];
        
        this->surface = SkSurface::MakeFromMTKView(this->context.get(), view, kTopLeft_GrSurfaceOrigin, f32([view sampleCount]), colorType, nullptr, nullptr);
        
        //this->surface = SkSurface::MakeFromCAMetalLayer(this->context.get(), layer, kTopLeft_GrSurfaceOrigin, sampleCount, colorType, nullptr, nullptr, &mtlDrawable);

        if(!this->surface) throw Exceptions::Exception("Failed to create Skia Surface.");

        this->canvas = this->surface->getCanvas();

        if(!this->surface) throw Exceptions::Exception("Failed to get Skia Canvas.");

        this->cursorManager = Shared<CursorManager>::make();
        this->cursorManager->setComponent(this->rootComponent);

        constexpr Float32 fw = 300.0f;
        constexpr Float32 fh = 30.0f;

        Point p = { f32(windowWidth) - fw - 10.0f, f32(windowHeight) - fh - 10.0f};
        Point s = {fw, fh };

        this->rootComponent->addChild(Component::Factory::create<FrameInfo>(p, s));

    }

    void MetalWindow::draw(const Float64 &deltaTime) {
        
        @autoreleasepool {
            
            assert(canvas && surface);
            
            canvas->save();
            canvas->scale(dpiScale, dpiScale);

            /*
            id<MTLCommandBuffer> buffer = [(__bridge id<MTLCommandQueue>)this->mtlQueue commandBuffer];
            // this->mtlDrawable = [(__bridge CAMetalLayer*)this->mtlLayer nextDrawable];
            this->rootComponent->draw(canvas, deltaTime);
            
            MTLRenderPassDescriptor *pass = [MTLRenderPassDescriptor renderPassDescriptor];
            pass.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);
            pass.colorAttachments[0].loadAction  = MTLLoadActionClear;
            pass.colorAttachments[0].storeAction = MTLStoreActionStore;
            pass.colorAttachments[0].texture = ((__bridge id<CAMetalDrawable>)this->mtlDrawable).texture;
            
            id<MTLRenderCommandEncoder> encoder = [buffer renderCommandEncoderWithDescriptor:pass];
            [encoder endEncoding];
            
            [buffer presentDrawable:(__bridge id<MTLDrawable>)this->mtlDrawable];
            [buffer commit];*/

            
            MTKView* view = (__bridge MTKView*)this->mtkView;
            
            id<MTLCommandBuffer> buffer = [(__bridge id<MTLCommandQueue>)this->mtlQueue commandBuffer];
            
            this->rootComponent->draw(canvas, deltaTime);
            
            MTLRenderPassDescriptor *pass = [view currentRenderPassDescriptor];
            
            context->flushAndSubmit();
            
            [buffer presentDrawable:[view currentDrawable]];
            [buffer commit];
            
            canvas->restore();

            
        }


        
    }

}
