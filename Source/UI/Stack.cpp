#include <Flux/UI/Stack.h>

namespace Flux {

    Stack::Stack(VAlignment vAlign, HAlignment hAlign) : vAlign(vAlign), hAlign(hAlign) {
        setColor(Color::fromHex(0));
    }

    void Stack::modified() {
        realign();
    }

    void Stack::childAdded(Component* child) {
        realign();
    }

    void Stack::parentLinked() {
        this->setSize(parent()->size());
    }

    HStack::HStack(VAlignment vAlign, HAlignment hAlign) : Stack(vAlign, hAlign) {

    }

    void HStack::realign() {

        const auto& ch = children();

        const Float32 totalWidth = childrenWidth();

        if(totalWidth > size().x){

            const Float32 spaceLeft = size().x - childSpacing * (f32(ch.size()) - 1);
            const Float32 newW = spaceLeft / f32(ch.size());

            for(auto& child : children()){
                child->setSize({ newW, child->size().y });
            }

        }

        Float32 newX = initialDrawX();

        for(auto* child : ch){

            const Float32 newY = childDrawY(child);

            child->setPosition({ newX, newY });

            newX += child->size().x + childSpacing;

        }

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

    VStack::VStack(VAlignment vAlign, HAlignment hAlign) : Stack(vAlign, hAlign) {

    }

    void VStack::realign() {

        const auto& ch = children();

        const Float32 totalHeight = childrenHeight();

        if(totalHeight > size().y){

            const Float32 spaceLeft = size().y - childSpacing * (f32(ch.size()) - 1);
            const Float32 newH = spaceLeft / f32(ch.size());

            for(auto& child : children()){
                child->setSize({ child->size().x, newH });
            }

        }

        Float32 newY = initialDrawY();

        for(auto* child : ch){

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

