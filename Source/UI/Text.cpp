#include <UI/Text.h>
#include "src/core/SkFontPriv.h"
#include "src/core/SkStrikeCache.h"
#include "src/core/SkStrikeSpec.h"

namespace Flux {

    void TextRenderer::draw(Graphics const& graphics) {

        if(textValue.isEmpty()) return;

        Point position = globalTransform().position;

        position.y += size().y - renderOffset;

        graphics.setAntiAliasing(true);
        graphics.setColor(color());
        graphics.drawText(textValue, position, fontSize, font);

    }

    void TextRenderer::setText(const String &text) {
        this->textValue = text;
        font.setSize(fontSize);
        recalcSize();
    }

    void TextRenderer::setTextSize(Float32 value) {
        fontSize = value;
        font.setSize(value);
        recalcSize();
    }

    void TextRenderer::colorChanged() {

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

        for(auto & glyph : glyphs){
            width += glyph->advanceX();
            auto off = f32(glyph->top() + glyph->height());
            height = Math::max(height, f32(glyph->height()) + off);
            offset = Math::max(offset, off);
        }

        if (Math::fneq(strikeToSourceScale, 1.0f)) {
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
                    *font.skiaFont(), w, h, renderOffset);

        setSize({ w, h });

    }

    TextRenderer::TextRenderer(String const& text, Float32 textSize) : font(Fonts::manrope) {

        setColor(Colors::white);
        setText(text);
        setTextSize(textSize);
        setReactive(false);

    }

    Text::Text(const Point &position, const Point &scale, String const& text, Float32 textSize,
               VAlignment vAlign, HAlignment hAlign) : Component(position, scale), hAlign(hAlign), vAlign(vAlign) {

        this->renderer = Factory::createComponent<TextRenderer>(text, textSize);

    }

    void Text::initialize() {
        addChild(renderer);
        realign();
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

    void Text::colorChanged() {
        renderer->setColor(color());
    }

    void Text::realign() {

        if(this->size().x < renderer->size().x){
            this->setSize({ renderer->size().x, size().y });
            Console::log("{}[Warning]: Text render size exceeds the container bounds! ({} at line {}){}\n",
                         Console::yellow, __FILE__, __LINE__, Console::reset);
        }

        if(this->size().y < renderer->size().y) {
            this->setSize({ size().x, renderer->size().y });

        }

        this->renderer->setPosition({ textX(), textY() });

    }

    Float32 Text::textX() const {
        
        const Float32 offset = 5.0f; //renderer->textSize() / 2.0f;

        if (hAlign == HAlignment::left) return offset;

        return hAlign == HAlignment::right ? size().x - renderer->size().x - offset : size().x / 2.0f - renderer->size().x / 2.0f;

    }

    Float32 Text::textY() const {
        
        const Float32 offset = 5.0f; //renderer->textSize() / 2.0f;

        if(vAlign == VAlignment::top) return offset;

        return vAlign == VAlignment::bottom ? size().y - renderer->size().y - offset : size().y / 2.0f - renderer->size().y / 2.0f;

    }

    void Text::draw(Graphics const& graphics) {
        
        renderer->draw(graphics);

    }

    void Text::setAlignment(VAlignment v, HAlignment h) {

        this->vAlign = v;
        this->hAlign = h;
        realign();

    }

}