#pragma once

#include <Nucleus/Nucleus.h>
#include <UI/Point.h>

class SkPath;

namespace Flux {

    using namespace Nucleus;

    class Path {

    public:

        Path();

        ~Path();

        Path(Path const& other);

        Path& operator=(Path const& other);

        void moveTo(Float32 x, Float32 y);

        void lineTo(Float32 x, Float32 y);

        void reset() const;

        NODISCARD SkPath* skPath() const { return path.pointer(); };

    private:

        Shared<SkPath> path;

    };

}
