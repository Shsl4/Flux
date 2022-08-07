#include <CursorManager.h>

namespace Flux::UserInterface {

    void CursorManager::onButtonDown(MouseButton button) {
        
        if (auto* component = master->getComponentAtPosition(getCursorPosition())) {
            auto r = stateMap.emplace(std::pair(button, component));
            component->onButtonDown(button, cursorX, cursorY);
        }
        
    }

    void CursorManager::onButtonUp(MouseButton button) {
        
        try {

            Reactive* target = stateMap.at(button);
            target->onButtonUp(button, cursorX, cursorY, master->getComponentAtPosition(getCursorPosition()));
            stateMap.erase(button);
        }
        catch (std::out_of_range const&) { }
        
    }

    void CursorManager::onScroll(Float64 xOffset, Float64 yOffset) const {
        
        if (auto* component = master->getComponentAtPosition(getCursorPosition())) {
            component->onScroll(xOffset, yOffset);
        }
        
    }

    void CursorManager::onCursorMoved(Float64 x, Float64 y) {

        // Update the cursor position.
        this->cursorX = x;
        this->cursorY = y;

        for (auto const& elem : stateMap) { elem.second->onDrag(elem.first, x, y); }

        // If a component is under the cursor.
        if (auto* component = master->getComponentAtPosition(getCursorPosition())) {

            // If the component under the cursor is already hovered, send a mouse movement message.
            if (hovered == component) {
                component->onCursorMoved(x, y);
                return;
            }

            // Otherwise, stop hovering the old component.
            clearHoveredComponent();

            // Send a hover message to the new component.
            component->onHover();
            hovered = component;
            return;
            
        }

        // No component is hovered, so stop hovering the old component if any.
        clearHoveredComponent();

    }

    void CursorManager::setCompound(Compound* value) { this->master = value; }

    void CursorManager::clearHoveredComponent() {
        
        if (hovered) {
            hovered->endHover();
            hovered = nullptr;
        }
        
    }

    bool CursorManager::getButtonFromReactive(const Reactive* value, MouseButton& out) {

        // Simply search through the map by value
        for (auto it = stateMap.begin(); it != stateMap.end(); ++it) {
            
            if (it->second == value) {
                
                out = it->first;
                return true;
                
            }
            
        }

        return false;

    }

    SkVector CursorManager::getCursorPosition() const {
        
        return {f32(cursorX), f32(cursorY)};
        
    }

}
