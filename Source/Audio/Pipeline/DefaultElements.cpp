#include <Audio/Pipeline/DefaultElements.h>
#include <Flux/Factory.h>

namespace Flux::Audio {

    PipelineInput::PipelineInput(): PipelineElement(0, 1) {
        
    }

    void PipelineInput::process(AudioBuffer<Float64> const& buffer) {
            
    }

    void PipelineInput::createComponent(Component* parent) {
        this->component = Factory::createComponent<InputNode>();
        parent->addChild(this->component);
    }

    Component* PipelineInput::getComponent() const {
        return this->component;
    }

    PipelineOutput::PipelineOutput(): PipelineElement(1, 0) {
    }

    void PipelineOutput::process(AudioBuffer<Float64> const& buffer) {

        assert(outputBuffer);
        memcpy(outputBuffer, (Float64*)buffer.getRawBuffers(), bufferSize() * sizeof(Float64));
        outputBuffer = nullptr;
            
    }

    void PipelineOutput::createComponent(Component* parent) {
        this->component = Factory::createComponent<OutputNode>();
        parent->addChild(this->component);
    }

    Component* PipelineOutput::getComponent() const { return this->component; }

    void PipelineOutput::prepareOutput(Float64* buffer) {
        this->outputBuffer = buffer;
    }
    
}
