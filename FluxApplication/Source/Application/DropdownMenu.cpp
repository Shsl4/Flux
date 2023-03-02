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
        
        const Float32 height = size().y;
        const Float32 arrowHeight = height / 6.0f;
        const Float32 rootHeight = height / 2.0f - arrowHeight / 2.0f;
        const Float32 maxHeight = height / 2.0f + arrowHeight / 2.0f;
        
        arrowTr.setRange(rootHeight, maxHeight);
        arrowRootY = rootHeight;
        arrowMaxY = maxHeight;
        
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
        
        if (arrowTr.state() == Animation::State::Idle) {
            arrowTr.play();
        }
        else {
            
            arrowTr.restartFromHere();
        
            if(arrowTr.playDirection() == Animation::PlayDirection::Normal) {
                
                arrowTr.setPlayDirection(Animation::PlayDirection::Reverse);
                
            }
            else {
                
                arrowTr.setPlayDirection(Animation::PlayDirection::Normal);
                
            }
        
        }

        
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

        Component::draw(canvas, deltaTime);
        
        animation.update(deltaTime);
        arrowTr.update(deltaTime);

        SkPaint paint;
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
        
    }
}
