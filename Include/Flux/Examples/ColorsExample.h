#pragma once

#include <Factory.h>
#include <UI/Stack.h>
#include <UI/Animation.h>

namespace Flux{

    class ColorShift : public Animation {

    public:

        explicit ColorShift(Component* component) : target(component){
            setLooping(true);
            setDuration(3.0f);
        }

        Float32 s = 0.0f;

    protected:

        void played() override {
            auto h = target->color().hsv().h / 360.0f;
            this->s = Random::randomFloat(0.0f, 1.0f);
            setProgress(h);
        }

        void process(Float64 deltaTime) override {

            HSV hsv = target->color().hsv();
            s = Math::clamp(s + f32(deltaTime) / 4.0f, 0.0f, 1.0f);
            auto p = progress();
            hsv.h = Math::lerp(0.0f, 360.0f, p);
            hsv.s = s;
            target->setColor(hsv.rgb());

        }

    private:

        Component* target = nullptr;

    };

    class ColorComp : public Component{

    public:

        ColorComp(const Point &p, const Point &s) : Component(p, s), shift(this) {

        }

        void initialize() override {

        }

        void hovered() override {

            shift.s = 0.0f;

        }

        void draw(SkCanvas *canvas, Float64 deltaTime) override {
            shift.update(deltaTime);
            Component::draw(canvas, deltaTime);
        }

        ColorShift shift;

    };

    class ColorsExample : public VStack {

    public:

        ColorsExample(const Point &p, const Point &s) : VStack(p, s, VAlignment::center, HAlignment::center) {
            setColor(Color::fromHex(0));
            setSpacing(10.0f);
        }

        void initialize() override {

            const size_t outMax = 10;

            for (size_t i = 0; i < outMax; ++i) {

                auto* hStack = Factory::createComponent<HStack>(Point::zero, Point(size().x, 50), VAlignment::center, HAlignment::center);
                hStack->setSpacing(10.0f);
                addChild(hStack);

                const size_t max = 15;

                for (size_t j = 0; j < max; ++j) {

                    auto* drawable = Factory::createComponent<ColorComp>(Point::zero, Point(50, 50));
                    hStack->addChild(drawable);
                    drawable->setColor(HSV(Math::lerp(0.0f, 360.0f, f32(i) / f32(outMax)),
                                           Math::lerp(0.0f, 1.0f, f32(max - j) / f32(max)), 1.0f).rgb());
                    drawable->shift.play();

                }

            }

        }

    protected:

        void parentModified() override {
            setSize(parent()->size());
        }

    };

}