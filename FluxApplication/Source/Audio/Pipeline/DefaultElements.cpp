#include <Audio/Pipeline/DefaultElements.h>

namespace Flux::Audio {

    PipelineInput::PipelineInput(): PipelineElement(0, 1) {
        
    }

    void PipelineInput::process(AudioBuffer<Float64> const& buffer) {
            
    }

    void PipelineInput::createComponent(UserInterface::Compound* parent) {
        this->component = parent->addChild<InputNode>();
    }

    UserInterface::Component* PipelineInput::getComponent() const {
        return this->component;
    }

    PipelineOutput::PipelineOutput(): PipelineElement(1, 0) {
    }

    void PipelineOutput::process(AudioBuffer<Float64> const& buffer) {

        assert(outputBuffer);
        memcpy(outputBuffer, (Float64*)buffer.getRawBuffers(), getBufferSize() * sizeof(Float64));
        outputBuffer = nullptr;
            
    }

    void PipelineOutput::createComponent(UserInterface::Compound* parent) {
        this->component = parent->addChild<OutputNode>();
    }

    UserInterface::Component* PipelineOutput::getComponent() const { return this->component; }

    void PipelineOutput::prepareOutput(Float64* buffer) {
        this->outputBuffer = buffer;
    }
    
}
