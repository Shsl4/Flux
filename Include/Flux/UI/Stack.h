#pragma once

#include <Flux/UI/Component.h>

namespace Flux {

    enum class HAlignment {
        left, center, right
    };

    enum class VAlignment {
        top, center, bottom
    };

    class Stack : public Component {

    public:

        Stack(VAlignment vAlign, HAlignment hAlign);

        NODISCARD FORCEINLINE Float32 spacing() const { return childSpacing; }

        void setSpacing(Float32 value) { this->childSpacing = value; }

        virtual void realign() = 0;

    protected:

        void childAdded(Component* child) override;

        void parentLinked() override;

        void modified() override;

        HAlignment hAlign = HAlignment::center;
        VAlignment vAlign = VAlignment::center;

        Float32 childSpacing = 0.0f;

    };

    class HStack : public Stack {

    public:

        HStack(VAlignment vAlign, HAlignment hAlign);

        void realign() override;

    protected:

        NODISCARD Float32 initialDrawX() const;
        NODISCARD Float32 childDrawY(Component* child) const;
        NODISCARD Float32 childrenWidth() const;

    };

    class VStack : public Stack {

    public:

        VStack(VAlignment vAlign, HAlignment hAlign);

        void realign() override;

    protected:

        NODISCARD Float32 childDrawX(Component* child) const;
        NODISCARD Float32 initialDrawY() const;
        NODISCARD Float32 childrenHeight() const;

    };
    
}
