#include <Flux/UI/Stack.h>

namespace Flux {

    Stack::Stack(Point const& p, Point const& s, VAlignment vAlign, HAlignment hAlign, bool stretch) : Component(p, s), vAlign(vAlign), hAlign(hAlign), bStretchChildren(stretch) {
        setColor(Colors::transparent);
    }

    void Stack::modified() {
        realign();
    }

    void Stack::parentModified() {
        realign();
    }

    void Stack::childWillDispose(Component *component) {
        realign();
    }

    void Stack::childAdded(Component *component) {
        realign();
    }

    HStack::HStack(Point const& p, Point const& s, VAlignment vAlign, HAlignment hAlign, bool stretch) : Stack(p, s, vAlign, hAlign, stretch) {

    }

    void HStack::realign() {

        const auto& ch = children();

        Float32 newX = initialDrawX();

        for(auto* child : ch){

            if(bStretchChildren){
                child->setSize({ child->size().x, size().y });
            }

            const Float32 newY = childDrawY(child);

            child->setPosition({ newX, newY });

            newX += child->size().x + childSpacing;

        }

    }

    void HStack::childAdded(Component* component) {
        Stack::childAdded(component);
        if(size().y < component->size().y) {
            this->setSize({ size().x, size().y + component->size().y });
        }
    }

    void HStack::parentLinked() {
        this->setSize({ parent()->size().x, size().y });
    }

    Float32 HStack::initialDrawX() const {

        if (hAlign == HAlignment::left) return 0.0f;

        const Float32 totalWidth = childrenWidth();

        return hAlign == HAlignment::right ? size().x - totalWidth : size().x / 2.0f - totalWidth / 2.0f;

    }

    Float32 HStack::childDrawY(Component* child) const {

        if(vAlign == VAlignment::top) return 0.0f;

        if (vAlign == VAlignment::center) return size().y / 2.0f - child->size().y / 2.0f;

        return size().y - child->size().y;

    }

    Float32 HStack::childrenWidth() const {

        const auto& ch = children();

        Float32 totalWidth = 0.0f;

        for(auto* child : ch){
            totalWidth += child->size().x;
        }

        totalWidth += childSpacing * f32((ch.size() - 1));

        return totalWidth;

    }


    VStack::VStack(Point const& p, Point const& s, VAlignment vAlign, HAlignment hAlign, bool stretch) : Stack(p, s, vAlign, hAlign, stretch) {

    }

    void VStack::realign() {

        const auto& ch = children();

        Float32 newY = initialDrawY();

        for(auto* child : ch){

            if(bStretchChildren){
                child->setSize({ size().x, child->size().y });
            }

            const Float32 newX = childDrawX(child);

            child->setPosition({ newX, newY });

            newY += child->size().y + childSpacing;

        }

    }

    Float32 VStack::childDrawX(Component *child) const {

        if(hAlign == HAlignment::left) return 0.0f;

        if (hAlign == HAlignment::center) return size().x / 2.0f - child->size().x / 2.0f;

        return size().x - child->size().x;

    }

    Float32 VStack::initialDrawY() const {

        if (vAlign == VAlignment::top) return 0.0f;

        const Float32 totalHeight = childrenHeight();

        return vAlign == VAlignment::bottom ? size().y - totalHeight : size().y / 2.0f - totalHeight / 2.0f;

    }

    Float32 VStack::childrenHeight() const {

        const auto& ch = children();
        Float32 totalHeight = 0.0f;

        for(auto* child : ch){
            totalHeight += child->size().y;
        }

        totalHeight += childSpacing * f32((ch.size() - 1));

        return totalHeight;

    }

}

