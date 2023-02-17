#pragma once

#include <Flux/UI/Components/Compound.h>
#include "Socket.h"
#include "../Audio/Pipeline/PipelineElement.h"
#include "FilterGraph.h"
#include "Audio/Pipeline/FilterElement.h"

namespace Flux {

    class Node : public UserInterface::Compound {

    public:
        
        Node(UInt inputs, UInt outputs, Float32 socketSize, Float32 baseWidth,
             Float32 baseHeight, Float32 headerSize, const UserInterface::LinearColor& headerColor);

        void initialize() override;

        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) override;

        void onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

        void cleanup() override;

        NODISCARD SkVector getInputSocketPosition(UInt channel);

        NODISCARD SkVector getOutputSocketPosition(UInt channel);

        void setElement(Audio::PipelineElement* value) { this->element = value; onElementSet(); }

        virtual void onElementSet();
        
        NODISCARD Audio::PipelineElement* getElement() const { return this->element; }
        
        NODISCARD UserInterface::LinearColor getHeaderColor() const { return this->headerColor; }

    protected:

        const Float32 socketSize = 25.0f;
        const Float32 baseWidth = 200.0f;
        const Float32 baseHeight = 50.0f;
        const Float32 headerSize = 5.0f;
        UserInterface::LinearColor headerColor = UserInterface::LinearColor::fromHex(0x8814f6ff);
        
    private:
        
        SkVector lastClickPos = {};
        
        const UInt inputs = 2;
        const UInt outputs = 2;

        Audio::PipelineElement* element = nullptr;

    };

    class FilterNode : public Node, public FilterGraphListener {

    public:

        FilterNode(UInt inputs, UInt outputs, Float32 socketSize, Float32 baseWidth,
                   Float32 baseHeight, Float32 headerSize, const UserInterface::LinearColor &headerColor);

        void onElementSet() override;

        void initialize() override;

        void draw(SkCanvas *canvas, Float64 deltaTime) override;

        void setPosition(const SkVector &value) override;

        void onValueChange(Float64 frequency, Float64 resonance) override;

    private:

        FilterElement* filterElement = nullptr;
        FilterGraph* drawer;
        static constexpr Float32 drawerHeight = 100.0f;

    };
    
}
