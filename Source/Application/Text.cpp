#include <UI/Text.h>
#include "src/core/SkFontPriv.h"
#include "src/core/SkStrikeCache.h"
#include "src/core/SkStrikeSpec.h"

namespace Flux {

    void TextRenderer::draw(SkCanvas *canvas, Float64 deltaTime) {

        const Point pos = globalTransform().position;

        canvas->drawSimpleText(&textValue[0], textValue.size(), SkTextEncoding::kUTF8,
                               pos.x + 2.0f, pos.y + size().y - renderOffset, font, paint);

    }

    void TextRenderer::setText(const String &text) {
        this->textValue = text;
        recalcSize();
    }

    void TextRenderer::setTextSize(Float32 value) {
        fontSize = value;
        font.setSize(fontSize);
        recalcSize();
    }

    void TextRenderer::colorChanged() {

        paint.setColor(color().skColor());

    }

    void measureText(const void* text, size_t length, SkTextEncoding encoding, SkFont& font, Float32& w, Float32& h, Float32& o) {

        SkAutoToGlyphs atg(font, text, length, encoding);

        const int glyphCount = atg.count();

        if (glyphCount == 0) return;

        const SkGlyphID* glyphIDs = atg.glyphs();

        auto [strikeSpec, strikeToSourceScale] = SkStrikeSpec::MakeCanonicalized(font);

        SkBulkGlyphMetrics metrics{strikeSpec};

        SkSpan<const SkGlyph*> glyphs = metrics.glyphs(SkSpan(glyphIDs, glyphCount));

        Float32 width = 0.0f;
        Float32 height = 0.0f;
        Float32 offset = 0.0f;

        int i = 0;
        for(auto & glyph : glyphs){
            width += glyph->advanceX();
            auto off = f32(glyph->top() + glyph->height());
            height = Math::max(height, f32(glyph->height() + off));
            offset = Math::max(offset, off);
        }

        if (strikeToSourceScale != 1) {
            width *= strikeToSourceScale;
            height *= strikeToSourceScale;
        }

        w = width;
        h = height;
        o = offset;

    }

    void TextRenderer::recalcSize() {

        if (textValue.isEmpty()) {
            setSize(Point::zero);
            return;
        }

        Float32 w = 0.0f, h = 0.0f;
        measureText(textValue.begin().get(), textValue.size(), SkTextEncoding::kUTF8,
                    font, w, h, renderOffset);

        setSize({ w, h });

    }

    TextRenderer::TextRenderer(String const& text, Float32 textSize) {

        setColor(Colors::white);
        setText(text);
        setTextSize(textSize);

    }

    Text::Text(const Point &position, const Point &scale, String const& text, Float32 textSize,
               VAlignment vAlign, HAlignment hAlign) : Component(position, scale), hAlign(hAlign), vAlign(vAlign) {

        this->renderer = Factory::createComponent<TextRenderer>(text, textSize);

    }

    void Text::initialize() {
        addChild(renderer);
    }

    bool Text::supportsChildren() const {
        return children().size() < 1;
    }

    void Text::setText(const String &text) {
        this->renderer->setText(text);
        realign();
    }

    void Text::setTextSize(Float32 value) {
        this->renderer->setTextSize(value);
        realign();
    }

    void Text::modified() {
        realign();
    }

    void Text::realign() {

        if(this->size().x < renderer->size().x || this->size().y < renderer->size().y){
            Console::log("{}[Warning]: Text render size exceeds the container bounds! ({} at line {}){}\n",
                         Console::yellow, __FILE__, __LINE__, Console::reset);
        }

        this->renderer->setPosition({ textX(), textY() });

    }

    Float32 Text::textX() const {

        if (hAlign == HAlignment::left) return 0.0f;

        return hAlign == HAlignment::right ? size().x - renderer->size().x : size().x / 2.0f - renderer->size().x / 2.0f;

    }

    Float32 Text::textY() const {

        if(vAlign == VAlignment::top) return 0.0f;

        return vAlign == VAlignment::bottom ? size().y - renderer->size().y : size().y / 2.0f - renderer->size().y / 2.0f;

    }

    void Text::draw(SkCanvas *canvas, Float64 deltaTime) {
        renderer->draw(canvas, deltaTime);
    }

}