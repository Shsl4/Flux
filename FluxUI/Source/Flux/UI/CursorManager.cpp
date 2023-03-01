#include <CursorManager.h>

namespace Flux::UI {

    CursorManager::CursorManager() {

        if(manager){
            throw Exceptions::Exception("Tried to create multiple instances of CursorManager.");
        }

        manager = this;
        
    }

    void CursorManager::buttonDown(const MouseButton button) {
        
        if (auto* component = componentAtPosition(master, cursorPosition())) {
            
            stateMap.add(button, component);
            component->onButtonDown(button, cursorX, cursorY);

            if(focused == component) { return; }
            
            if(focused) {
                focused->endFocus();
            }
            
            focused = component;
            focused->onFocus();
            
        }
        
    }

    void CursorManager::doubleClick(const MouseButton button) const {

        if (auto* component = componentAtPosition(master, cursorPosition())) {
            component->onDoubleClick(button, cursorX, cursorY);
        }
        
    }

    void CursorManager::buttonUp(const MouseButton button) {
        
        try {

            Reactive* target = stateMap[button];
            target->onButtonUp(button, cursorX, cursorY, componentAtPosition(master, cursorPosition()));
            stateMap.removeByKey(button);
            
        }
        catch (Exceptions::BadKey const&) { }
        
    }

    void CursorManager::scroll(const Float64 xOffset, const Float64 yOffset) const {
        
        if (auto* component = componentAtPosition(master, cursorPosition())) {
            component->onScroll(xOffset, yOffset);
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
            elem.getValue()->onDrag(elem.getKey(), x, y, deltaX, deltaY);
        }

        // If a component is under the cursor.
        if (auto* component = componentAtPosition(master, cursorPosition())) {

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

    CursorManager* CursorManager::cursorManager() { return manager; }
    
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
            
            hovered->endHover();
            hovered = nullptr;
        }
        
    }

    bool CursorManager::buttonFromReactive(const Reactive* value, MouseButton& out) const {

        // Simply search through the map by value
        for (auto it = stateMap.begin(); it != stateMap.end(); ++it) {
            
            if (it->getValue() == value) {
                
                out = it->getKey();
                return true;
                
            }
            
        }

        return false;

    }

    Component* CursorManager::componentAtPosition(Component* root, Point const& p) {
        
        const auto children = root->children();
        
        for (size_t i = children.size(); i > 0; --i) {

            auto const& child = children[i - 1];

            if(componentAtPosition(child, p)) { return child; }

            if(child->inBounds(p)) { return child; }
            
        }

        if(root->inBounds(p)) { return root; }
        
        return nullptr;
        
    }

    Point CursorManager::cursorPosition() const {
        
        return { f32(cursorX), f32(cursorY) };
        
    }

}
