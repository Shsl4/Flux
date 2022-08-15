#include <Audio/Pipeline/DefaultElements.h>

namespace Flux::Audio {

    PipelineInput::PipelineInput(): PipelineElement(0, 1) {

        this->component = SharedPointer<InputNode>::make(this);
        
    }

    void PipelineInput::process(AudioBuffer<Float64> const& buffer) {
            
    }

    SharedPointer<UserInterface::Component> PipelineInput::getComponent() const {
        return this->component;
    }

    PipelineOutput::PipelineOutput(): PipelineElement(1, 0) {
        this->component = SharedPointer<OutputNode>::make(this);
    }

    void PipelineOutput::process(AudioBuffer<Float64> const& buffer) {

        fassert(outputBuffer);
        memcpy(outputBuffer, (Float64*)buffer.getRawBuffers(), getBufferSize() * sizeof(Float64));
        outputBuffer = nullptr;
            
    }

    SharedPointer<UserInterface::Component> PipelineOutput::getComponent() const { return this->component; }

    void PipelineOutput::prepareOutput(Float64* buffer) {
        this->outputBuffer = buffer;
    }
    
}
