#pragma once

#include <Flux/Core/Flux.h>

namespace Flux {
    
    enum class NodeType
    {
        Literal,
        Argument,
        Executable
    };

    enum class ArgumentType : UInt
    {
        String,
        Int64,
        Double,
        Bool
    };
    
}