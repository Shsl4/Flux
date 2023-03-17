#pragma once

#include <Flux/UI/Component.h>
#include <Audio/Pipeline/Pipeline.h>

namespace Flux {

    using namespace UI;
    
    class LinkResolver : public Component {

    public:

        void draw(SkCanvas *canvas, Float64 deltaTime) override;

        Audio::Pipeline* pipeline;

    private:

        Float64 linePhase = 0.0;

    };

}