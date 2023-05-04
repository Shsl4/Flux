#include <UI/SceneComponent.h>
#include <Examples/ColorsExample.h>
#include <Factory.h>

namespace Flux {

    SceneComponent::SceneComponent(Point const& p, Point const& s): Component(p, s), scaling(1.0f), zoomAnim(scaling, offset) {
        
        setColor(Colors::transparent);

        Point textSize = { 100, 20 };
        
        this->offsetText = Factory::createComponent<Text>(s - Point(100, 20), textSize, "(0, 0)", 14.0f, VAlignment::bottom, HAlignment::right);
        this->scaleText = Factory::createComponent<Text>(s - textSize - Point(0, 20), textSize, "x1.00", 14.0f, VAlignment::bottom, HAlignment::right);

    }


    void SceneComponent::modified() {

        Point textSize = { 100, 20 };

        this->offsetText->setPosition(size() - textSize);
        this->scaleText->setPosition(size() - textSize - Point(0, 20));

    }


    void SceneComponent::initialize() {
        
    }

    void SceneComponent::handleScroll(Float64 deltaX, Float64 deltaY) {
        this->scroll(deltaX, deltaY);
    }

    void SceneComponent::handleButtonDown(MouseButton button) {

        Component* component = this;

        if(button != MouseButton::Right) {
            component = master->componentAtPosition(cursorPosition());
        }
        
        if (component) {
            
            stateMap.add(button, component);
            setButtonState(component, button, true);
            component->buttonDown(button, cursorX, cursorY);

            if(CursorManager::focused == component) { return; }
            
            if(CursorManager::focused) {
                CursorManager::focused->lostFocus();
            }

            if(!Factory::valid(component)) {
                stateMap.removeByKey(button);
                CursorManager::hovered = nullptr;
                CursorManager::focused = nullptr;
                return;
            }
            
            CursorManager::focused = component;
            CursorManager::focused->focused();
            
        }
        
    }

    void SceneComponent::handleDrag(Float64 deltaX, Float64 deltaY) {

        Point newPoint = toWorld(cursorPosition());
        
        for (auto const& elem : stateMap) {
            
            elem.value()->drag(elem.key(), newPoint.x, newPoint.y, deltaX, deltaY);
            
        }
        
    }

    void SceneComponent::draw(Graphics const& graphics) {

        zoomAnim.update(graphics.deltaTime());
        
        graphics.save();
        graphics.translate({-offset.x, -offset.y});
        graphics.scale({scaling, scaling});
        
        Component::draw(graphics);

        graphics.restore();

        this->offsetText->setText(String::format("({}, {})", i32(offset.x), i32(offset.y)));
        this->scaleText->setText(String::format("x{.2}", scaling));

        offsetText->draw(graphics);
        scaleText->draw(graphics);
        
    }

    void SceneComponent::drag(const MouseButton button, Float64 x, Float64 y, const Float64 deltaX, const Float64 deltaY) {

        if(button == MouseButton::Right) {

            this->zoomAnim.add({f32(deltaX), f32(deltaY)});
            
        }
        
    }

    FORCEINLINE Float32 ssg(const Float32 f) {
        return f > 0.0f ? 1.0f : -1.0f;
    }

    void SceneComponent::scroll(Float64 deltaX, const Float64 deltaY) {

        const Point preScale = toWorld(cursorPosition());
        
        this->scaling = Math::clamp(scaling + 0.25f * ssg(f32(deltaY)), 0.25f, 1.5f);

        const Point postScale = toWorld(cursorPosition());
        
        zoomAnim.setTargetValue(this->scaling, offset + (preScale - postScale) * scaling);
        
    }
    
    void SceneComponent::doubleClick(const MouseButton button, const Float64 x, const Float64 y) {
        
        
    }

    Component* SceneComponent::componentAtPosition(Point const& p)  {
        auto* c = Component::componentAtPosition(toWorld(p));
        return c ? c : this;
    }
    
    Point SceneComponent::toWorld(Point const& p) const {
        return (p + offset) / scaling;
    }

}
