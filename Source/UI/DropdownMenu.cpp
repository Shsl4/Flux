#include <UI/DropdownMenu.h>
#include <Flux/Factory.h>
#include <skia/include/effects/SkGradientShader.h>

namespace Flux {

    DropdownMenu::DropdownMenu(Point const& p, Point const& s) : Component(p, s), arrowTr(arrowRootY, arrowMaxY) {

        setColor(Colors::transparent);
        arrowTr.setDuration(0.1);
        arrowTr.setPlayDirection(Animation::PlayDirection::Reverse);
        
        const Float32 height = size().y;
        const Float32 arrowHeight = height / 6.0f;
        const Float32 rootHeight = height / 2.0f - arrowHeight / 2.0f;
        const Float32 maxHeight = height / 2.0f + arrowHeight / 2.0f;

        arrowTr.setRange(rootHeight, maxHeight);
        arrowRootY = rootHeight;
        arrowMaxY = maxHeight;

        this->button = Factory::createComponent<Button>(Point::zero, size(), "(No choices)");
        this->stack = Factory::createComponent<VStack>(Point::zero, Point(s.x, 0.0f), VAlignment::top, HAlignment::left);

    }

    void DropdownMenu::initialize() {
        
        addChild(stack);
        addChild(button);

        button->setColorScheme(scheme);
        button->label()->setTextSize(size().y / 2.0f);
        button->label()->setAlignment(VAlignment::center, HAlignment::left);
        button->setAction([this]{

            arrowTr.switchDirection();
            arrowTr.restartFromHere();
            stack->setReactive(!stack->reactive());
            stack->setVisible(!stack->visible());

        });

        stack->setColor(Colors::transparent);
        stack->setReactive(false);
        stack->setVisible(false);
        stack->setPosition({ 0, size().y });

    }
    
    void DropdownMenu::focused() {

        Console::log("{} received focus.\n", Type::name(*this));
        
    }

    void DropdownMenu::lostFocus() {

        Console::log("{} lost focus\n", Type::name(*this));
        
    }

    void DropdownMenu::choiceCallback(Function<void(size_t)> const& f) {
        this->callback = f;
    }

    void DropdownMenu::draw(SkCanvas* canvas, Float64 deltaTime) {

        button->draw(canvas, deltaTime);

        arrowTr.update(deltaTime);

        SkPaint paint;
        
        paint.setColor(color().skColor());
        const auto t = globalTransform();
        
        paint.setColor(SkColors::kWhite);
        paint.setStrokeWidth(2.0f);
        paint.setStrokeCap(SkPaint::kRound_Cap);

        const Float32 width = size().x;
        const Float32 height = size().y;

        const Float32 arrowHeight = height / 6.0f;
        const Float32 arrowWidth = arrowHeight * 2.0f;

        const Float32 offset = width * 0.025f;
        const Float32 drawX = t.position.x + width - arrowWidth - offset;

        canvas->drawLine(drawX, arrowRootY, drawX + arrowWidth / 2.0f, arrowMaxY, paint);
        canvas->drawLine(drawX + arrowWidth / 2.0f, arrowMaxY, drawX + arrowWidth, arrowRootY, paint);

        stack->draw(canvas, deltaTime);
        
    }

    void DropdownMenu::choiceSelected(size_t i) {

        arrowTr.switchDirection();
        arrowTr.restartFromHere();
        stack->setReactive(!stack->reactive());
        stack->setVisible(!stack->visible());
        
        if(i == currentSelection) return;

        this->currentSelection = i;

        button->label()->setText(choices[currentSelection]);

        if(callback){
            callback(currentSelection);
        }

    }

    void DropdownMenu::setChoices(const MutableArray<String> &array) {

        if(array.size() == 0) return;

        this->choices = array;

        stack->disposeAllChildren();

        stack->setSize({ stack->size().x, f32(choices.size()) * size().y });

        size_t i = 0;

        for(auto& choice : choices){

            auto* btn = Factory::createComponent<Button>(Point::zero, size(), choice);
            btn->setColorScheme(ColorScheme::fromColor(scheme.lighter));
            btn->label()->setTextSize(size().y / 2.0f);
            btn->label()->setAlignment(VAlignment::center, HAlignment::left);
            btn->setAction([=, this]{
                choiceSelected(i);
            });

            ++i;

            stack->addChild(btn);

        }

        currentSelection = 0;
        button->label()->setText(choices[currentSelection]);

    }

    void DropdownMenu::modified() {

        this->button->setSize(size());
        this->stack->setSize({ size().x, stack->size().y });

    }

}
