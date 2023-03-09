#include <Application/DropdownMenu.h>

namespace Flux {

    DropdownMenu::DropdownMenu() : Component({ {}, { 200, 25 }}), arrowTr(arrowRootY, arrowMaxY),
                                   defaultColor(Color::fromHex(0x60606060)),
                                   hoverColor(Color::fromHex(0x50505050)),
                                   pressedColor(Color::fromHex(0x45454545)) {

        setColor(this->defaultColor);
        animation.setTarget(this);
        animation.setDuration(0.1);
        arrowTr.setDuration(0.1);
        arrowTr.setPlayDirection(Animation::PlayDirection::Reverse);
        
        const Float32 height = size().y;
        const Float32 arrowHeight = height / 6.0f;
        const Float32 rootHeight = height / 2.0f - arrowHeight / 2.0f;
        const Float32 maxHeight = height / 2.0f + arrowHeight / 2.0f;
        
        arrowTr.setRange(rootHeight, maxHeight);
        arrowRootY = rootHeight;
        arrowMaxY = maxHeight;
        
    }

    void DropdownMenu::initialize() {
        
        Component* child = addChild(Factory::create<Component>(Point(0, size().y), Point(size().x, 500)));
        child->setColor(Color::fromHex(0xffffffff));
        child->setActive(false);

    }
    
    void DropdownMenu::focused() {

        Reactive::focused();
        
        Console::log("{} received focus.\n", Type::name(*this));
        
    }

    void DropdownMenu::lostFocus() {

        Reactive::lostFocus();

        Console::log("{} lost focus\n", Type::name(*this));
        
    }

    void DropdownMenu::hovered() {

        Reactive::hovered();
        updateColor();
        
    }
    
    void DropdownMenu::unHovered() {

        Reactive::unHovered();
        updateColor();
        
    }

    void DropdownMenu::buttonDown(MouseButton button, Float64 x, Float64 y) {

        Reactive::buttonDown(button, x, y);
        updateColor();
    }

    void DropdownMenu::buttonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget) {

        Reactive::buttonUp(button, x, y, upTarget);
        updateColor();

        if(button != MouseButton::Left || upTarget != this) return;

        arrowTr.switchDirection();
        arrowTr.restartFromHere();
        opened = !opened;
        
    }

    void DropdownMenu::choiceCallback(Function<void(String const&)> const& f) {
        this->callback = f;
    }
    
    void DropdownMenu::updateColor() {

        Color color = defaultColor;
        
        if (isPressed(MouseButton::Left)) {
            color = pressedColor;
        }
        else if (isHovered()) {
            color = hoverColor;
        }

        animation.setTargetColor(color);
        animation.restart();
        
    }
    
    void DropdownMenu::draw(SkCanvas* canvas, Float64 deltaTime) {
        
        animation.update(deltaTime);
        arrowTr.update(deltaTime);

        SkPaint paint;
        
        paint.setColor(color().skColor());
        const auto t = globalTransform();
        const SkRect rect = SkRect::MakeXYWH(t.position.x, t.position.y, t.size.x, t.size.y);
        canvas->drawRect(rect, paint);
        
        paint.setColor(SkColors::kWhite);
        paint.setStrokeWidth(2.0f);
        paint.setStrokeCap(SkPaint::kRound_Cap);

        const Float32 width = size().x;
        const Float32 height = size().y;

        const Float32 arrowHeight = height / 6.0f;
        const Float32 arrowWidth = arrowHeight * 2.0f;

        const Float32 offset = width * 0.05f;
        const Float32 drawX = width - arrowWidth - offset;

        canvas->drawLine(drawX, arrowRootY, drawX + arrowWidth / 2.0f, arrowMaxY, paint);
        canvas->drawLine(drawX + arrowWidth / 2.0f, arrowMaxY, drawX + arrowWidth, arrowRootY, paint);

        if (opened) {
            for(const auto& child : children()) {
                child->draw(canvas, deltaTime);
            }
        }
        
    }
}
