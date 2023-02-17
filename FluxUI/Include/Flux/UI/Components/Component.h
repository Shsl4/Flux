#pragma once

#include <Flux/UI/LinearColor.h>
#include <Flux/UI/Reactive.h>

#pragma warning(push)
#pragma warning(disable : 4244)
#include <skia/core/SkCanvas.h>
#pragma warning(pop)

namespace Flux::UserInterface {

    enum class Flow {
        Output = 0, Input
    };

    class Compound;

    class Component : public Reactive {

        friend Compound;
        
    public:

        ~Component() override;
        
        virtual void initialize();

        virtual void cleanup();
        
        virtual void draw(SkCanvas* canvas, Float64 deltaTime);
        
        virtual void setColor(LinearColor const& value);

        virtual void setPosition(SkVector const& value);

        virtual void setScale(SkVector const& value);
        
        void setBounds(Float32 x, Float32 y, Float32 width, Float32 height);
        
        void discard();

        NODISCARD SkVector getAbsolutePosition() const;
        
        NODISCARD SkVector getAbsoluteCenteredPosition() const;

        NODISCARD SkVector getPosition() const;

        NODISCARD SkVector getScale() const;

        NODISCARD LinearColor getColor() const;

        NODISCARD bool isInBounds(SkVector const& value) const;

        NODISCARD UInt getDepth() const;
        
        NODISCARD Compound* getParent() const;

    private:
        
        SkVector position = {};
        SkVector scale = {};
        LinearColor color = Colors::white;
        Compound* parent = nullptr;
        bool pendingDiscard = false;
        
    };
    
}
