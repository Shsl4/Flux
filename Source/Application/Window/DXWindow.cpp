#include <Application/Window/DXWindow.h>
#include <Flux/UI/Component.h>
#include <Flux/Factory.h>
#include <d3dx12.h>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Flux {
    
    void DXWindow::initDX12(Int windowWidth, Int windowHeight) {
        
        const HWND rawWindow = glfwGetWin32Window(this->handle);
        IDXGISwapChain1* swapChainPtr = nullptr;
        IDXGIFactory4* factoryPtr = nullptr;
        ID3D12Device* devicePtr = nullptr;

        nthrowif(FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factoryPtr))), "Failed to create DGXI Factory");

        this->factory = gr_cp(factoryPtr);

        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapterPtr;
        
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, adapterPtr.ReleaseAndGetAddressOf()); ++adapterIndex) {
            
            DXGI_ADAPTER_DESC1 desc;
            nthrowif(FAILED(adapterPtr->GetDesc1(&desc)), "Failed to get adapter description!");

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(adapterPtr.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&devicePtr)))){
                this->device = gr_cp(devicePtr);
                break;
            }
            
        }

#if !defined(NDEBUG)
        if (!adapterPtr)
        {
            if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(adapterPtr.ReleaseAndGetAddressOf()))))
            {
                throw std::runtime_error("WARP12 not available. Enable the 'Graphics Tools' optional feature");
            }
        }
#endif

        if (!adapterPtr)
        {
            throw std::runtime_error("No Direct3D 12 device found");
        }
        
        this->adapter = gr_cp(adapterPtr.Detach());
        
        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = windowWidth;
        swapChainDesc.Height = windowHeight;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc;
        fsDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
        fsDesc.Windowed = true;
        fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
        
        factory->CreateSwapChainForHwnd(adapter.get(), rawWindow, &swapChainDesc, &fsDesc, nullptr, &swapChainPtr);

        this->swapChain = gr_cp(swapChainPtr);

    }

    void DXWindow::setupCanvas(Int width, Int height) {

        Int framebufferWidth;
        Int framebufferHeight;

        glfwGetFramebufferSize(this->handle, &framebufferWidth, &framebufferHeight);
        
        GrGLFramebufferInfo framebufferInfo;

        framebufferInfo.fFBOID = 0;

        // 0x8058 = GL_RGBA8
        framebufferInfo.fFormat = 0x8058;

        constexpr SkColorType colorType = kRGBA_8888_SkColorType;

        const GrBackendRenderTarget backendRenderTarget(framebufferWidth, framebufferHeight, 4, 8, framebufferInfo);

        this->surface = SkSurface::MakeFromBackendRenderTarget(this->context.get(), backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr);

        if(!this->surface) throw Exceptions::Exception("Failed to create Skia Surface.");

        this->canvas = this->surface->getCanvas();

        if(!this->canvas) throw Exceptions::Exception("Failed to get Skia Canvas.");
        
    }
    
    DXWindow::DXWindow(const String& title, Int windowWidth, Int windowHeight) {

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
        
        initDX12(windowWidth, windowHeight);

        GrD3DBackendContext ctx;
        
        ctx.fAdapter = adapter;
        ctx.fDevice = device;
        
        this->context = GrDirectContext::MakeDirect3D(ctx);

        setupCanvas(windowWidth, windowHeight);

        if(!this->context) throw Exceptions::Exception("Failed to create Skia metal context.");
        
        this->rootComponent = Factory::createComponent<Component>(Point(0, 0), Point(f32(windowWidth),f32(windowHeight)));

        this->cursorManager = Shared<CursorManager>::make();
        this->cursorManager->setComponent(this->rootComponent);
        
    }

    
    void DXWindow::draw(Float64 const& deltaTime) {

        glfwMakeContextCurrent(this->handle);

        canvas->save();
        canvas->scale(dpiScale, dpiScale);
        
        this->rootComponent->draw(this->canvas, deltaTime);

        context->flush();
        canvas->restore();
        
        glfwSwapBuffers(this->handle);
        
    }
    
    void DXWindow::resized(Int width, Int height) {

        
        
    }
    
}
