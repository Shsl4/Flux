#pragma once

#include <Flux/Core/Flux.h>
#include <Flux/UI/LinearColor.h>

#pragma warning(push)
#pragma warning(disable : 4244)
#include <skia/core/SkCanvas.h>
#pragma warning(pop)

namespace Flux::UserInterface {

    class Component {

    public:
        
        virtual void initialize();
        
        virtual void draw(SkCanvas* canvas);
        
        virtual ~Component() = default;

        void setBounds(Float32 x, Float32 y, Float32 width, Float32 height);

        void setColor(LinearColor const& value);

        void setPosition(SkVector const& value);

        void setScale(SkVector const& value);

        NODISCARD SkVector getPosition() const;

        NODISCARD SkVector getScale() const;

        NODISCARD LinearColor getColor() const;

    private:

        SkVector position = {};
        SkVector scale = {};
        LinearColor color = Colors::white;
        
    };
    
}
