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

        Stack(Point const& p, Point const& s, VAlignment vAlign, HAlignment hAlign, bool stretch = false);

        NODISCARD FORCEINLINE Float32 spacing() const { return childSpacing; }

        void setSpacing(Float32 value) {
            this->childSpacing = value;
            realign();
        }

        virtual void realign() = 0;

    protected:

        void childWillDispose(Component *component) override;

        void parentModified() override;

        void modified() override;

        void childAdded(Component *component) override;

        HAlignment hAlign = HAlignment::center;
        VAlignment vAlign = VAlignment::center;

        Float32 childSpacing = 0.0f;
        bool bStretchChildren = false;

    };

    class HStack : public Stack {

    public:

        HStack(Point const& p, Point const& s, VAlignment vAlign, HAlignment hAlign, bool stretch = false);

        void realign() override;

    protected:

        NODISCARD Float32 initialDrawX() const;
        NODISCARD Float32 childDrawY(Component* child) const;
        NODISCARD Float32 childrenWidth() const;

    };

    class VStack : public Stack {

    public:

        VStack(Point const& p, Point const& s, VAlignment vAlign, HAlignment hAlign, bool stretch = false);

        void realign() override;

    protected:

        NODISCARD Float32 childDrawX(Component* child) const;
        NODISCARD Float32 initialDrawY() const;
        NODISCARD Float32 childrenHeight() const;

    };
    
}
