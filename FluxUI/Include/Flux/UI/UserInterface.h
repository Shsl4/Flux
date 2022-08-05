#pragma once

#include <Flux/Core/Flux.h>
#include <Flux/Core/Memory/SharedPointer.h>

class SkSurface;
class GrDirectContext;
class SkCanvas;

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

    class MasterView {
        
        friend class UserInterface;
        
    public:

        MasterView() = default;

        virtual ~MasterView() = default;
        
        static SharedPointer<MasterView> makeGL(Int width, Int height, OpenGLParams const& params);
        
        virtual void draw();

    private:
        
        SkSurface* surface = nullptr;
        SkCanvas* canvas = nullptr;
        GrDirectContext* context = nullptr;
        
    };
    
    class UserInterface {

        
        
    };
    
}
