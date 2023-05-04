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

#include <skia/include/core/SkColorSpace.h>

namespace Flux {

    MetalWindow::MetalWindow(const String &title, Int windowWidth, Int windowHeight, Component* rootComponent) {

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

        this->handle = glfwCreateWindow(windowWidth, windowHeight, title.begin().get(), nullptr, nullptr);

        if (!this->handle) {
            throw Exceptions::Exception("Failed to create window handle!");
        }

        glfwSetKeyCallback(this->handle, &Window::inputCallback);
        glfwSetMouseButtonCallback(this->handle, &Window::mouseCallback);
        glfwSetScrollCallback(this->handle, &Window::scrollCallback);
        glfwSetCursorPosCallback(this->handle, &Window::cursorCallback);
        glfwSetWindowSizeCallback(this->handle, &Window::resizeCallback);
        glfwSetWindowCloseCallback(this->handle, &Window::closeCallback);
                        
        NSWindow* rawWindow = glfwGetCocoaWindow(this->handle);
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        id<MTLCommandQueue> queue = [device newCommandQueue];
        
        MTKView* view = [[MTKView alloc] initWithFrame: { 0, 0, f32(windowWidth), f32(windowHeight) } device:device];
        [[rawWindow contentView] addSubview:view];
        
        this->mtkView = view;
        this->mtlQueue = queue;
        
        this->dpiScale = f32([rawWindow backingScaleFactor]);

        this->context = GrDirectContext::MakeMetal(device, queue);

        if(!this->context) throw Exceptions::Exception("Failed to create Skia metal context.");

        if(rootComponent) {
            this->component = rootComponent;
        }
        else {
            this->component = Factory::createComponent<Component>(Point(0, 0), Point(f32(windowWidth), f32(windowHeight)));
        }
        
        if(auto* cast = dynamic_cast<CursorManager*>(component)) {
            
            this->manager = cast;
            this->manager->setComponent(rootComponent);
            
        }
        else {
            
            this->manager = Factory::createCursorManager();
            this->manager->setComponent(this->component);
            
        }

    }

    void MetalWindow::resized(Int width, Int height) {

        auto view = (__bridge MTKView*)this->mtkView;

        CGRect frame = view.frame;
        frame.size.height = height;
        frame.size.width = width;
        view.frame = frame;

        component->setSize({f32(width), f32(height)});
        
    }

    void MetalWindow::draw(const Float64 &deltaTime) {
        
        @autoreleasepool {
                    
            auto view = (__bridge MTKView*)this->mtkView;
            auto queue = (__bridge id<MTLCommandQueue>)this->mtlQueue;
            id<MTLCommandBuffer> buffer = [queue commandBuffer];

            auto surface = SkSurface::MakeFromMTKView(this->context.get(), view, kTopLeft_GrSurfaceOrigin, f32([view sampleCount]), kBGRA_8888_SkColorType, nullptr, nullptr);
            
            auto* canvas = surface->getCanvas();

            graphics.updateDeltaTime(deltaTime);
            graphics.updateCanvas(canvas);
            graphics.setColor(ColorScheme::onyx.base);
            graphics.drawRect(Point::zero, {f32(view.drawableSize.width), f32(view.drawableSize.height)});
            graphics.scale({dpiScale, dpiScale});

            this->component->draw(graphics);
                        
            context->flushAndSubmit();

            [buffer presentDrawable:[view currentDrawable]];
            [buffer commit];
            
        }
        
    }

}
