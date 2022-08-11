#pragma once

#include "../../Application/Node.h"

namespace Flux::Audio {
    
    class InputNode : public Node {
        
    public:
        
        InputNode(PipelineElement* elem) : Node(elem, 0, 1, 25.0, 100.0, 0.0, 5.0, UserInterface::Colors::white) {}
        
        
    };

    class OutputNode : public Node {
        
    public:
        
        OutputNode(PipelineElement* elem) : Node(elem, 1, 0, 25.0, 100.0, 0.0, 5.0, UserInterface::Colors::white) {}
        
    };
        
    class PipelineInput : public PipelineElement {
        
    public:
        
        PipelineInput();

        void process(AudioBuffer<Float64> const& buffer) override;

        NODISCARD SharedPointer<UserInterface::Component> getComponent() const override;

    private:

        SharedPointer<InputNode> component;

    };

    class PipelineOutput : public PipelineElement {

    public:

        PipelineOutput();

        void process(AudioBuffer<Float64> const& buffer) override;

        NODISCARD SharedPointer<UserInterface::Component> getComponent() const override;

    private:
        
        friend class Pipeline;

        void prepareOutput(Float64* buffer);
        
        Float64* outputBuffer = nullptr;
        SharedPointer<OutputNode> component;

    };
    
}