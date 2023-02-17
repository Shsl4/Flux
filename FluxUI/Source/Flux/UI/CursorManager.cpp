#include <CursorManager.h>

namespace Flux::UserInterface {

    CursorManager::CursorManager() {

        if(manager){
            throw Exceptions::Exception("Tried to create multiple instances of CursorManager.");
        }

        manager = this;
        
    }

    void CursorManager::onButtonDown(MouseButton button) {
        
        if (auto* component = master->getComponentAtPosition(getCursorPosition())) {
            
            stateMap.add(button, component);
            component->onButtonDown(button, cursorX, cursorY);

            if(focused == component) { return; }
            
            if(focused) { focused->endFocus(); }
            
            focused = component;
            focused->onFocus();
            
        }
        
    }

    void CursorManager::onDoubleClick(MouseButton button) const {

        if (auto* component = master->getComponentAtPosition(getCursorPosition())) {
            component->onDoubleClick(button, cursorX, cursorY);
        }
        
    }

    void CursorManager::onButtonUp(MouseButton button) {
        
        try {

            Reactive* target = stateMap[button];
            target->onButtonUp(button, cursorX, cursorY, master->getComponentAtPosition(getCursorPosition()));
            stateMap.removeByKey(button);
            
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
        this->lastCursorX = cursorX;
        this->lastCursorY = cursorY;
        
        this->cursorX = x;
        this->cursorY = y;

        const Float64 deltaX = lastCursorX - cursorX;
        const Float64 deltaY = lastCursorY - cursorY;

        for (auto const& elem : stateMap) {
            elem.getValue()->onDrag(elem.getKey(), x, y, deltaX, deltaY);
        }

        // If a component is under the cursor.
        if (auto* component = master->getComponentAtPosition(getCursorPosition())) {

            // If the component under the cursor is already hovered, send a mouse movement message.
            if (hovered == component) {
                component->onCursorMoved(x, y, deltaX, deltaY);
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

    CursorManager* CursorManager::getCursorManager() {
        return manager;
    }
    
    void CursorManager::setCompound(Compound* value) { this->master = value; }

    void CursorManager::notifyDestruction(const Reactive* reactive) {

        if(reactive == hovered) {
            hovered = nullptr;
        }
        
        if(reactive == focused) {
            focused = nullptr;
        }
        
    }
    
    void CursorManager::clearHoveredComponent() {
        
        if (hovered) {
            hovered->endHover();
            hovered = nullptr;
        }
        
    }

    bool CursorManager::getButtonFromReactive(const Reactive* value, MouseButton& out) {

        // Simply search through the map by value
        for (auto it = stateMap.begin(); it != stateMap.end(); ++it) {
            
            if (it->getValue() == value) {
                
                out = it->getKey();
                return true;
                
            }
            
        }

        return false;

    }

    SkVector CursorManager::getCursorPosition() const {
        
        return { f32(cursorX), f32(cursorY) };
        
    }

}
