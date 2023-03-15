#pragma once

#include <Flux/UI/Component.h>
#include "../Audio/Pipeline/PipelineElement.h"
#include "FilterGraph.h"
#include "Audio/Pipeline/FilterElement.h"

namespace Flux {

    using namespace UI;

    class Node : public Component {

    public:
        
        void initialize() override;
        
        Node(UInt inputs, UInt outputs, Float32 socketSize, Float32 baseWidth,
             Float32 baseHeight, Float32 headerSize, const Color& headerColor);
        
        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void buttonDown(MouseButton button, Float64 x, Float64 y) override;

        void drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;
        
        NODISCARD Point getInputSocketPosition(UInt channel);

        NODISCARD Point getOutputSocketPosition(UInt channel);

        void setElement(Audio::PipelineElement* value) { this->element = value; onElementSet(); }

        virtual void onElementSet();
        
        NODISCARD Audio::PipelineElement* getElement() const { return this->element; }
        
        NODISCARD Color getHeaderColor() const { return this->headerColor; }

    protected:
        
        void willDispose() override;

        const Float32 socketSize = 25.0f;
        const Float32 baseWidth = 200.0f;
        const Float32 baseHeight = 50.0f;
        const Float32 headerSize = 5.0f;
        Color headerColor = Color::fromHex(0x8814f6ff);
        
    private:
        
        Point lastClickPos = {};
        
        const UInt inputs = 2;
        const UInt outputs = 2;

        Audio::PipelineElement* element = nullptr;

    };

    class FilterNode : public Node, public FilterGraphListener {

    public:

        FilterNode(UInt inputs, UInt outputs, Float32 socketSize, Float32 baseWidth,
                   Float32 baseHeight, Float32 headerSize, const Color &headerColor);

        void onElementSet() override;
        
        void draw(SkCanvas *canvas, Float64 deltaTime) override;

        void onValueChange(Float64 frequency, Float64 resonance) override;
        
        void initialize() override;
    
    protected:
        
        void modified() override;

    private:

        FilterElement* filterElement = nullptr;
        FilterGraph* drawer;
        static constexpr Float32 drawerHeight = 100.0f;

    };
    
}
