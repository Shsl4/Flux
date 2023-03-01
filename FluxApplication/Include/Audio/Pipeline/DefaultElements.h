#pragma once

#include "../../Application/Node.h"

namespace Flux::Audio {
    
    class InputNode : public Node {
        
    public:
        
        InputNode() : Node(0, 1, 25.0, 100.0, 0.0, 5.0, Colors::white) {}
        
        
    };

    class OutputNode : public Node {
        
    public:
        
        OutputNode() : Node(1, 0, 25.0, 100.0, 0.0, 5.0, Colors::white) {}
        
    };
        
    class PipelineInput : public PipelineElement {
        
    public:
        
        PipelineInput();

        void process(AudioBuffer<Float64> const& buffer) override;

        void createComponent(Component* parent) override;

        NODISCARD Component* getComponent() const override;

    private:

        InputNode* component;

    };

    class PipelineOutput : public PipelineElement {

    public:

        PipelineOutput();

        void process(AudioBuffer<Float64> const& buffer) override;

        void createComponent(Component* parent) override;

        NODISCARD Component* getComponent() const override;

    private:
        
        friend class Pipeline;

        void prepareOutput(Float64* buffer);
        
        Float64* outputBuffer = nullptr;
        OutputNode* component;

    };
    
}