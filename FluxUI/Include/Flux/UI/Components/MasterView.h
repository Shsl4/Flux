#pragma once

#include <Flux/UI/Components/Compound.h>
#include <Flux/UI/Reactive.h>

class GrDirectContext;

namespace Flux::UserInterface {
    
    struct OpenGLParams {
        
        OpenGLParams(const Int framebufferId, const Int sampleCount, const Int stencilBits)
            : framebufferId(framebufferId),
              sampleCount(sampleCount),
              stencilBits(stencilBits) {}

        Int framebufferId = 0;
        Int sampleCount = 0;
        Int stencilBits = 0;
        
    };
    
    class MasterView : public Compound {
    
    public:
        
        void initialize() override;

        void draw(SkCanvas* canvas, Float64 deltaTime) override;
        
        static SharedPointer<MasterView> makeGL(Int width, Int height, OpenGLParams const& params);
    
    private:
        
        SkSurface* surface = nullptr;
        SkCanvas* canvas = nullptr;
        GrDirectContext* context = nullptr;
        
    };
    
}
