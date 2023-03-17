#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <MetalKit/MetalKit.h>

#include <Flux/Application/Window/MetalWindow.h>
#include <Flux/UI/Component.h>
#include <Flux/Factory.h>

namespace Flux {

    MetalWindow::MetalWindow(const String &title, Int windowWidth, Int windowHeight) {

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

        this->handle = glfwCreateWindow(windowWidth, windowHeight, title.begin().get(), nullptr, nullptr);

        if (!this->handle) {
            throw Exceptions::Exception("Failed to createComponent window handle!");
        }

        glfwSetKeyCallback(this->handle, &Window::inputCallback);
        glfwSetMouseButtonCallback(this->handle, &Window::mouseCallback);
        glfwSetScrollCallback(this->handle, &Window::scrollCallback);
        glfwSetCursorPosCallback(this->handle, &Window::cursorCallback);
        glfwSetWindowCloseCallback(this->handle, &Window::closeCallback);
        
        NSWindow* rawWindow = glfwGetCocoaWindow(this->handle);
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        id<MTLCommandQueue> queue = [device newCommandQueue];
        
        MTKView* view = [[MTKView alloc] initWithFrame: { 0, 0, f32(windowWidth), f32(windowHeight) } device:device];
        [[rawWindow contentView] addSubview:view];
        
        this->mtkView = view;
        this->mtlQueue = queue;
        
        this->dpiScale = [rawWindow backingScaleFactor];
                
        this->rootComponent = Factory::createComponent<Component>(Point(0, 0), Point(windowWidth * dpiScale,
                                                                                     windowHeight * dpiScale));
        
        this->context = GrDirectContext::MakeMetal(device, queue);

        if(!this->context) throw Exceptions::Exception("Failed to createComponent Skia metal context.");

        constexpr SkColorType colorType = kBGRA_8888_SkColorType;
        Float32 sampleCount = [view sampleCount];

        this->cursorManager = Shared<CursorManager>::make();
        this->cursorManager->setComponent(this->rootComponent);

        constexpr Float32 fw = 300.0f;
        constexpr Float32 fh = 30.0f;

        Point p = { f32(windowWidth) - fw - 10.0f, f32(windowHeight) - fh - 10.0f};
        Point s = {fw, fh };

        this->rootComponent->addChild(Flux::Factory::createComponent<FrameInfo>(p, s));

    }

    void MetalWindow::draw(const Float64 &deltaTime) {
        
        @autoreleasepool {
                    
            MTKView* view = (__bridge MTKView*)this->mtkView;
            id<MTLCommandQueue> queue = (__bridge id<MTLCommandQueue>)this->mtlQueue;
            id<MTLCommandBuffer> buffer = [queue commandBuffer];
            
            auto surface = SkSurface::MakeFromMTKView(this->context.get(), view, kTopLeft_GrSurfaceOrigin, f32([view sampleCount]), kBGRA_8888_SkColorType, nullptr, nullptr);
            
            auto* canvas = surface->getCanvas();
            
            canvas->scale(dpiScale, dpiScale);

            this->rootComponent->draw(canvas, deltaTime);
                        
            context->flushAndSubmit();
            
            id<MTLCommandBuffer> commandBuffer = [queue commandBuffer];
            [commandBuffer presentDrawable:[view currentDrawable]];
            [commandBuffer commit];
            
        }
        
    }

}
