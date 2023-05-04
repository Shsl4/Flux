#include <UI/Font.h>
#include <skia/include/core/SkFont.h>

namespace Flux {

    Font::Font(const Nucleus::String &path) {

        this->font = Nucleus::Shared<SkFont>::make(SkTypeface::MakeFromFile(path.cString()));

        if (!font.valid() || !font->getTypeface()) {
            throw Exceptions::Exception("Invalid font file or path provided!");
        }

    }

    void Font::setSize(Float32 newSize) {
        this->font->setSize(newSize);
    }

    Float32 Font::size() const { return this->font->getSize(); }

    Font::~Font() {

    }

}