#pragma once

#include <Flux/Core/Flux.h>
#include <Flux/Core/Memory/String.h>

namespace Flux {
    
    class Identifiable {
    
    public:

        virtual ~Identifiable() = default;
    
        /*!
         * Returns the name of the class.
         *
         *  \return The name of the class.
         */
        NODISCARD String getClassName() const;

        /*!
         * Returns a debug description of the object.
         *
         *  \return The object description
         */
        NODISCARD virtual String getDescription() const;

        template <typename T>
        NODISCARD FORCEINLINE T* cast() { return dynamic_cast<T*>(this); }

    };
    
}
