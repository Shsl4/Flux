#pragma once

#include <Flux/UI/Stack.h>
#include <Flux/Factory.h>
#include <UI/Fonts.h>

namespace Flux {

    class TextRenderer : public Component {

    public:

        TextRenderer(String const& text, Float32 textSize);

        void draw(SkCanvas *canvas, Float64 deltaTime) override;

        void setText(String const& text);

        void setTextSize(Float32 value);

        NODISCARD FORCEINLINE String text() const { return this->textValue; }

        NODISCARD FORCEINLINE Float32 textSize() const { return this->fontSize; }

    protected:

        void colorChanged() override;

    private:

        void recalcSize();

        Float32 fontSize = 12.0f;
        Float32 renderOffset = 0.0f;
        String textValue = nullptr;
        SkPaint paint;
        SkFont font;

    };


    class Text : public Component {

    public:

        Text(Point const& position, Point const& scale, String const& text, Float32 textSize,
             VAlignment vAlign = VAlignment::center, HAlignment hAlign = HAlignment::left);

        void initialize() override;

        void setText(String const& text);

        void setTextSize(Float32 value);

        void draw(SkCanvas *canvas, Float64 deltaTime) override;

        void setAlignment(VAlignment vAlign, HAlignment hAlign);

        NODISCARD FORCEINLINE String text() const { return this->renderer->text(); }

        NODISCARD FORCEINLINE Float32 textSize() const { return this->renderer->textSize(); }

    protected:
        
        void colorChanged() override;

        void realign();

        void modified() override;

        NODISCARD bool supportsChildren() const override;

    private:

        HAlignment hAlign = HAlignment::left;
        VAlignment vAlign = VAlignment::center;
        TextRenderer* renderer = nullptr;

        NODISCARD Float32 textX() const;
        NODISCARD Float32 textY() const;

    };

}
