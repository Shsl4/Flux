#pragma once

#include <Flux/UI/Component.h>
#include <Flux/UI/Text.h>

namespace Flux {

    class FrameInfo : public Component {

    public:

        FrameInfo(Point const& p, Point const& s) : Component(p, s) {

            setColor(Color::fromHex(0x606060ff));

        }

        void initialize() override {

            this->text = Flux::Factory::createComponent<Text>(Point(10, 0), size(), nullptr, 12);
            this->addChild(text);

        }

        void draw(SkCanvas* canvas, Float64 deltaTime) override {

            const Float64 currentTime = glfwGetTime();
            ++frameCount;

            if(currentTime - lastTime >= 1.0) {

                lastFrameTime = (1000.0 / static_cast<Float64>(frameCount));
                frameCount = 0;
                lastTime += 1.0;

            }

            SkPaint paint;

            paint.setColor(color().skColor());

            const Point pos = globalTransform().position;
            const SkRect rect = SkRect::MakeXYWH(pos.x, pos.y, size().x, size().y);

            canvas->drawRoundRect(rect, 10, 10, paint);

            String value = String::format("Draw: {} FPS ({}ms)", 1000.0 * (1.0 / lastFrameTime), lastFrameTime);
            text->setText(value);
            text->draw(canvas, deltaTime);

        }

    private:

        Text* text = nullptr;
        Float64 lastTime = 0.0;
        UInt frameCount = 0;
        Float64 lastFrameTime = 0.0;

    };

}