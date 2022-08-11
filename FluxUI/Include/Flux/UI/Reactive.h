﻿#pragma once

#include <Flux/Core/Flux.h>
#include <Flux/Core/Tools/Console/Console.h>
#include <Flux/Core/Interfaces/Identifiable.h>

namespace Flux::UserInterface {

    enum class MouseButton : UInt {

        Left = 0,
        Right = 1,
        Middle = 2,
        Other
        
    };

    class Reactive : public Identifiable {
        
    public:
        
        virtual ~Reactive() = default;
        
        /**
         * \brief Function called when a mouse button is pressed
         * \param button The mouse button pressed
         */
        virtual void onButtonDown(MouseButton button, Float64 x, Float64 y);
        
        /**
         * \brief Function called when a mouse button is pressed twice quickly
         * \param button The mouse button pressed
         */
        virtual void onDoubleClick(MouseButton button, Float64 x, Float64 y);

        /**
         * \brief Function called when a mouse button is released
         * \param button The mouse button pressed
         * \param x The cursor x position
         * \param y The cursor y position
         * \param upTarget The Reactive on which the button was released
         */
        virtual void onButtonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget);

        /**
         * \brief Function called when the mouse wheel is scrolled
         * \param xOffset The x scroll offset
         * \param yOffset The y scroll offset
         */
        virtual void onScroll(Float64 xOffset, Float64 yOffset);
        
        /**
         * \brief Function called when the mouse drags the component
         * \param button The mouse button pressed
         * \param x The cursor x position
         * \param y The cursor y position
         * \param deltaX The cursor x delta position
         * \param deltaY The cursor y delta position
         */
        virtual void onDrag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY);
     
        /**
         * \brief Function called when the cursor moves in bounds of the component
         * \param x The cursor x position
         * \param y The cursor y position
         * \param deltaX The cursor x delta position
         * \param deltaY The cursor y delta position
         */
        virtual void onCursorMoved(Float64 x, Float64 y, Float64 deltaX, Float64 deltaY);

        /**
         * \brief Function called when the cursor begins hovering the component
         */
        virtual void onHover();
     
        /**
         * \brief Function called when the cursor stops hovering the component
         */
        virtual void endHover();

        virtual void onFocus();

        virtual void endFocus();

    };
    
}

