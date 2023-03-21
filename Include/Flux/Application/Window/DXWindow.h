#pragma once

#define _NOMINMAX
#include <Application/Window/Window.h>
#include <skia/include/gpu/d3d/GrD3DBackendContext.h>
#include <d3d12.h>
#include <dxgi1_2.h>
#include <dxgi1_4.h>

namespace Flux {

    class DXWindow : public Window {
        
    public:

        DXWindow(const String &title, Int windowWidth, Int windowHeight);
        
        void draw(Float64 const& deltaTime) override;

        void resized(Int width, Int height) override;

    private:

        void initDX12(Int windowWidth, Int windowHeight);
        
        void setupCanvas(Int width, Int height);

        gr_cp<IDXGISwapChain1> swapChain = nullptr;
        gr_cp<IDXGIFactory4> factory = nullptr;
        gr_cp<ID3D12Device> device = nullptr;
        gr_cp<IDXGIAdapter1> adapter = nullptr;
        sk_sp<GrDirectContext> context = nullptr;
        
        sk_sp<SkSurface> surface = nullptr;
        SkCanvas* canvas = nullptr;
        Float32 dpiScale = 1.0f;
        
    };

    
}
