#include <UI/CursorManager.h>
#include <Flux/Factory.h>

namespace Flux {

    void CursorManager::buttonDown(const MouseButton button) {
        
        if (auto* component = master->componentAtPosition(cursorPosition())) {
            
            stateMap.add(button, component);
            component->pressedState[static_cast<size_t>(button)] = true;
            component->buttonDown(button, cursorX, cursorY);

            if(focused == component) { return; }
            
            if(focused) {
                focused->lostFocus();
            }

            if(!Factory::valid(component)) {
                stateMap.removeByKey(button);
                hovered = nullptr;
                focused = nullptr;
                return;
            }
            
            focused = component;
            focused->focused();
            
        }
        
    }

    void CursorManager::doubleClick(const MouseButton button) const {

        if (auto* component = master->componentAtPosition(cursorPosition())) {
            component->doubleClick(button, cursorX, cursorY);
        }
        
    }

    void CursorManager::buttonUp(const MouseButton button) {
        
        try {

            Reactive* target = stateMap[button];
            target->pressedState[static_cast<size_t>(button)] = false;
            target->buttonUp(button, cursorX, cursorY, master->componentAtPosition(cursorPosition()));
            stateMap.removeByKey(button);
            
        }
        catch (Exceptions::BadKey const&) { }
        
    }

    void CursorManager::scroll(const Float64 xOffset, const Float64 yOffset) const {
        
        if (auto* component = master->componentAtPosition(cursorPosition())) {
            component->scroll(xOffset, yOffset);
        }
        
    }

    void CursorManager::cursorMoved(const Float64 x, const Float64 y) {

        // Update the cursor position.
        this->lastCursorX = cursorX;
        this->lastCursorY = cursorY;
        
        this->cursorX = x;
        this->cursorY = y;

        const Float64 deltaX = lastCursorX - cursorX;
        const Float64 deltaY = lastCursorY - cursorY;

        for (auto const& elem : stateMap) {
            elem.value()->drag(elem.key(), x, y, deltaX, deltaY);
        }

        // If a component is under the cursor.
        if (auto* component = master->componentAtPosition(cursorPosition())) {

            // If the component under the cursor is already bHovered, send a mouse movement message.
            if (hovered == component) {
                component->cursorMoved(x, y, deltaX, deltaY);
                return;
            }

            // Otherwise, stop hovering the old component.
            clearHoveredComponent();

            // Send a hover message to the new component.
            component->hovered();
            hovered = component;
            return;
            
        }

        // No component is bHovered, so stop hovering the old component if any.
        clearHoveredComponent();

    }

    void CursorManager::setComponent(Component* value) { this->master = value; }

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

            hovered->unHovered();
            hovered = nullptr;

        }
        
    }

    bool CursorManager::buttonFromReactive(const Reactive* value, MouseButton& out) const {

        // Simply search through the map by value
        for (auto it = stateMap.begin(); it != stateMap.end(); ++it) {
            
            if (it->value() == value) {
                
                out = it->key();
                return true;
                
            }
            
        }

        return false;

    }

    Point CursorManager::cursorPosition() const {
        
        return { f32(cursorX), f32(cursorY) };
        
    }

}
