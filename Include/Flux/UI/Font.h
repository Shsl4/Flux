#pragma once

#include <Nucleus/Nucleus.h>

class SkFont;

namespace Flux {

    using namespace Nucleus;

    class Font {

    public:

        explicit Font(String const& path);

        ~Font();

        NODISCARD SkFont* skiaFont() const { return font.pointer(); }

        NODISCARD Float32 size() const;

        void setSize(Float32 newSize);

    private:

        Nucleus::Shared<SkFont> font = {};

    };

}
