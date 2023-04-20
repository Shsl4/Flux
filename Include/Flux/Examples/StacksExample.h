#pragma once

#include <Factory.h>
#include <Application/Application.h>
#include <UI/Stack.h>
#include <UI/Text.h>
#include <UI/Button.h>
#include <UI/Animation.h>

namespace Flux{


    class CoolAnimation : public Animation{

    public:

        CoolAnimation(class Stack* p) : stack(p){
            setAutoReverse(true);
            setDuration(1.0f);
            setLooping(true);
        }

    protected:

        void process(Float64 deltaTime) override;
        Stack* stack;

    };

    template<typename T>
    class CoolStack : public T {

    public:

        CoolStack(Point const& p, Point const& s, VAlignment vAlign, HAlignment hAlign) : T(p, s, vAlign, hAlign, true), anim(this) {
            anim.play();
        }

        void draw(SkCanvas *canvas, Float64 deltaTime) override {

            VStack::draw(canvas, deltaTime);

            anim.update(deltaTime);

        }

        CoolAnimation anim;

    };

    void CoolAnimation::process(Float64 deltaTime) {
        stack->setSpacing(Math::easeInOut(0.0f, 100.0f, f32(progress()), 2));
    }

    class StacksExample : public CoolStack<VStack> {

    public:

        StacksExample(Point const& p, Point const& s) : CoolStack(p, s, VAlignment::center, HAlignment::center) {

        }

        void initialize() override {

            auto* stack3 = Factory::createComponent<HStack>(Point::zero, Point(size().x, 100.0f), VAlignment::center, HAlignment::center);
            auto* stack4 = Factory::createComponent<HStack>(Point::zero, Point(size().x, 100.0f), VAlignment::center, HAlignment::center);
            auto* stack5 = Factory::createComponent<HStack>(Point::zero, Point(size().x, 100.0f), VAlignment::center, HAlignment::center);
            auto* stack2 = Factory::createComponent<HStack>(Point::zero, Point(size().x, 100.0f), VAlignment::center, HAlignment::center);

            setSpacing(50);

            stack2->setColor(Color::randomColor());
            stack3->setColor(Color::randomColor());
            stack4->setColor(Color::randomColor());
            stack5->setColor(Color::randomColor());

            auto* text = Factory::createComponent<Text>(Point::zero, Point(500, 200), "Hello, world!", 50.0f, VAlignment::center, HAlignment::center);
            auto* text2 = Factory::createComponent<Text>(Point::zero, Point(500, 200), "c[0] = -7", 50.0f, VAlignment::center, HAlignment::center);
            auto* text3 = Factory::createComponent<Text>(Point::zero, Point(500, 200), "Text!", 50.0f, VAlignment::center, HAlignment::center);

            addChild(stack2);
            addChild(stack3);
            addChild(stack4);
            addChild(stack5);

            auto button = Factory::createComponent<Button>(Point::zero, Point(150, 50), "Exit");

            button->setAction([&](auto*) {
                Application::instance()->stop();
            });

            button->setColorScheme(ColorScheme::deepPurple);

            stack2->addChild(text);
            stack3->addChild(text2);
            stack4->addChild(button);
            stack5->addChild(text3);

        }

    public:

        void draw(SkCanvas *canvas, Float64 deltaTime) override {

            canvas->translate(posX, posY);
            CoolStack::draw(canvas, deltaTime);

        }

    protected:
        void parentModified() override {
            setSize(parent()->size());
        }

        Float32 posX = 0.0f, posY = 0.0f;

    };

}
