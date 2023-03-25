#pragma once

#include <Nucleus/Nucleus.h>

using namespace Nucleus;

namespace Flux {

    enum class MouseButton : size_t {

        Left = 0,
        Right = 1,
        Middle = 2,
        Size

        
    };

    struct Key {
     
         Key(Int key, Int mods);
     
         Int key;
         bool bShift;
         bool bCaps;
         bool bCtrl;
         bool bSuper;
         bool bAlt;
         bool bNumLock;
  
    };
 
    class Reactive {

         friend class CursorManager;
        
    public:

        enum class State : size_t {

            Idle,
            Hovered,
            Pressed,
         
        };
     
        virtual ~Reactive() = default;
        
        /**
         * \brief Function called when a mouse button is pressed
         * \param button The mouse button pressed
         */
        virtual void buttonDown(MouseButton button, Float64 x, Float64 y);
        
        /**
         * \brief Function called when a mouse button is pressed twice quickly
         * \param button The mouse button pressed
         */
        virtual void doubleClick(MouseButton button, Float64 x, Float64 y);

        /**
         * \brief Function called when a mouse button is released
         * \param button The mouse button pressed
         * \param x The cursor x position
         * \param y The cursor y position
         * \param upTarget The Reactive on which the button was released
         */
        virtual void buttonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget);

        /**
         * \brief Function called when the mouse wheel is scrolled
         * \param deltaX The x scroll offset
         * \param deltaY The y scroll offset
         */
        virtual void scroll(Float64 deltaX, Float64 deltaY);
        
        /**
         * \brief Function called when the mouse drags the component
         * \param button The mouse button pressed
         * \param x The cursor x position
         * \param y The cursor y position
         * \param deltaX The cursor x delta position
         * \param deltaY The cursor y delta position
         */
        virtual void drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY);
     
        /**
         * \brief Function called when the cursor moves in bounds of the component
         * \param x The cursor x position
         * \param y The cursor y position
         * \param deltaX The cursor x delta position
         * \param deltaY The cursor y delta position
         */
        virtual void cursorMoved(Float64 x, Float64 y, Float64 deltaX, Float64 deltaY);

        virtual void keyDown(Key const& key);

        virtual void keyUp(Key const& key);

        /**
         * \brief Function called when the cursor begins hovering the component
         */
        virtual void hovered();
     
        /**
         * \brief Function called when the cursor stops hovering the component
         */
        virtual void unHovered();

        virtual void focused();

        virtual void lostFocus();
     
        NODISCARD FORCEINLINE bool isFocused() const { return this->bFocused; }
     
        NODISCARD FORCEINLINE bool isHovered() const { return this->bHovered; }
     
        NODISCARD FORCEINLINE bool isPressed(MouseButton button) const { return pressedState[static_cast<size_t>(button)]; }

        NODISCARD FORCEINLINE bool reactive() const { return this->bActive; }

        void setReactive(bool state);

    private:

        StaticArray<bool, i64(MouseButton::Size)> pressedState = {};
        bool bHovered = false;
        bool bFocused = false;
        bool bActive = true;

    };
    
}

