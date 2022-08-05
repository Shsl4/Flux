#pragma once

#include <Flux/Core/Flux.h>

#define BASIC_FORMATTER(type) \
template <> \
class Formatter<type> {\
public:\
static void format(std::ostream& stream, type const& elem) { stream << elem; }\
};\
template <> \
class Formatter<const type> {\
public:\
static void format(std::ostream& stream, type const& elem) { stream << elem; }\
}

namespace Flux {

    template <typename T>
    class Formatter;

    template <typename T>
    class Formatter<T*> {
    public:
        static void format(std::ostream& stream, T* const& elem) {
            if (elem) { stream << elem; }
            else { stream << "nullptr"; }
        }
    };

    template <typename T>
    class Formatter<const T*> {
    public:
        static void format(std::ostream& stream, const T* const& elem) {
            if (elem) { stream << elem; }
            else { stream << "nullptr"; }
        }

    };

    BASIC_FORMATTER(char);
    
    BASIC_FORMATTER(size_t);

    BASIC_FORMATTER(Float64);
    
    BASIC_FORMATTER(Float32);

    BASIC_FORMATTER(UInt);

    BASIC_FORMATTER(Int);

    BASIC_FORMATTER(Int64);

    BASIC_FORMATTER(std::string);

}
