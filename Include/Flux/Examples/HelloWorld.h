#pragma once

#include <UI/Stack.h>
#include <UI/Text.h>

namespace Flux {

    class HelloWorld : public Component {

    public:

        HelloWorld(const Point &p, const Point &s) : Component(p, s) {

            this->text = Factory::createComponent<Text>(Point::zero, s, "Hello, world!",
                                                        50.0f, VAlignment::center, HAlignment::center);
            setColor(Colors::transparent);
        }

        void initialize() override {
            addChild(text);
        }

    protected:
        
        void modified() override {
            text->setSize(size());
        }
        
        void parentModified() override {
            setSize(parent()->size());
        }

    private:

        Text* text;

    };

}
