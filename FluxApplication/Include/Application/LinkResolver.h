#pragma once

#include <Flux/UI/Components/Component.h>
#include <Audio/Pipeline/Pipeline.h>

namespace Flux {

    class LinkResolver : public UserInterface::Component {

    public:

        void initialize() override;

        void draw(SkCanvas *canvas, Float64 deltaTime) override;

        Audio::Pipeline* pipeline;

    private:

        Float64 linePhase = 0.0;

    };

}