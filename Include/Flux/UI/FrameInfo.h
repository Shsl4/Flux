#pragma once

#include <Flux/UI/Component.h>
#include <Flux/UI/Text.h>

namespace Flux {

    class FrameInfo : public Component {

    public:

        FrameInfo(Point const& p, Point const& s) : Component(p, s) {

        }

        void initialize() override {

            this->text = Flux::Factory::createComponent<Text>(Point::zero, size(), nullptr, 14.0f, VAlignment::top, HAlignment::left);
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

            const auto fps = i64(1000.0 * (1.0 / lastFrameTime));
            String value = String::format("{} FPS", fps > 100000 ? 0 : fps);
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