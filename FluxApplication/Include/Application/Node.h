#pragma once

#include <Flux/UI/Components/Compound.h>
#include <Flux/Core/Math/Math.h>
#include "Socket.h"
#include "../Audio/Pipeline/PipelineElement.h"

namespace Flux {

    class Node : public UserInterface::Compound {

    public:
        
        Node(Audio::PipelineElement* element, UInt inputs, UInt outputs, Float32 socketSize, Float32 baseWidth,
             Float32 baseHeight, Float32 headerSize, const UserInterface::LinearColor& headerColor);

        void initialize() override;

        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) override;

        void onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

        void cleanup() override;

        NODISCARD SkVector getInputSocketPosition(UInt channel);

        NODISCARD SkVector getOutputSocketPosition(UInt channel);

        NODISCARD Audio::PipelineElement* getElement() const { return this->element; }
        
        NODISCARD UserInterface::LinearColor getHeaderColor() const { return this->headerColor; }

    private:
        
        SkVector lastClickPos = {};
        
        const UInt inputs = 2;
        const UInt outputs = 2;

        const Float32 socketSize = 25.0f;
        const Float32 baseWidth = 200.0f;
        const Float32 baseHeight = 50.0f;
        const Float32 headerSize = 5.0f;
        UserInterface::LinearColor headerColor = UserInterface::LinearColor::fromHex(0x8814f6ff);

        Audio::PipelineElement* element;

    };
    
}
