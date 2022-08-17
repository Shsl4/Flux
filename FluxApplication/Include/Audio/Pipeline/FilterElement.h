#pragma once

#include <Audio/Pipeline/PipelineElement.h>
#include <Flux/Audio/Effects/Filters/Filter.h>

namespace Flux{
    
    class FilterElement : public Audio::PipelineElement {

    public:

        FilterElement(UInt inChannels, UInt outChannels);

        NODISCARD OwnedArray<Audio::Filter> const& getFilters() const { return this->filters; }

    protected:

        OwnedArray<Audio::Filter> filters;

    };

}
