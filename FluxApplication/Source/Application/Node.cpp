#include <Application/Node.h>

#include "Audio/Pipeline/FilterElement.h"

namespace Flux {
    
    Node::Node(const UInt inputs, const UInt outputs, const Float32 socketSize, const Float32 baseWidth,
               const Float32 baseHeight, const Float32 headerSize, const UserInterface::LinearColor& headerColor)
        : inputs(inputs), outputs(outputs), socketSize(socketSize), baseWidth(baseWidth), baseHeight(baseHeight),
          headerSize(headerSize), headerColor(headerColor) { }

    void Node::initialize() {
            
        const UInt count = inputs > outputs ? inputs : outputs;
        const Float32 ioHeight = socketSize * f32(count);

        setScale( { baseWidth,  baseHeight + headerSize + ioHeight } );
        setColor(UserInterface::LinearColor::fromHex(0x606060ff));

        for (UInt i = 0; i < inputs; ++i) {

            const auto child = addChild<Socket>();
            child->setScale({ socketSize, socketSize });
            child->setPosition({ 0.0f, baseHeight + headerSize + socketSize * f32(i) });
            child->setFlow(UserInterface::Flow::Input);
            child->setDataType(DataType::Audio, i);
            
        }

        for (UInt i = 0; i < outputs; ++i) {

            const auto child = addChild<Socket>();
            child->setScale({ socketSize, socketSize });
            child->setPosition({ getScale().x() - socketSize, baseHeight + headerSize + socketSize * f32(i) });
            child->setFlow(UserInterface::Flow::Output);
            child->setDataType(DataType::Audio, i);
            
        }

    }

    void Node::draw(SkCanvas* canvas, Float64 deltaTime) {

        SkPaint paint;
            
        paint.setColor(getColor().toSkColor());

        const SkVector pos = getAbsolutePosition();
        SkRect rect = SkRect::MakeXYWH(pos.x(), pos.y(), getScale().x(), getScale().y());

        canvas->drawRect(rect, paint);
            
        paint.setColor(headerColor.toSkColor());
        rect = SkRect::MakeXYWH(pos.x(), pos.y(), getScale().x(), headerSize);
        canvas->drawRect(rect, paint);

        Compound::draw(canvas, deltaTime);
            
    }

    void Node::onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) {
            
        if (button == UserInterface::MouseButton::Left) {

            lastClickPos = SkVector::Make(f32(x), f32(y)) - getPosition();
                
        }
            
    }

    void Node::onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        if (button == UserInterface::MouseButton::Left) { setPosition(SkVector::Make(f32(x), f32(y)) - lastClickPos); }
            
    }

    void Node::cleanup() {

        Compound::cleanup();

        if(element) {
            element->discard();
        }
        
    }

    SkVector Node::getOutputSocketPosition(UInt channel) {
        SkVector e = getAbsolutePosition();
        SkVector v = { getScale().x(), baseHeight + headerSize + socketSize * f32(channel) + socketSize / 2.0f};
        return e + v;
    }

    void Node::onElementSet() {}

    SkVector Node::getInputSocketPosition(UInt channel) {

        SkVector e = getAbsolutePosition();
        SkVector v = { socketSize / 2.0f, socketSize / 2.0f + baseHeight + headerSize + socketSize * f32(channel) };
        return e + v;

    }

    void FilterNode::initialize() {
        
        Node::initialize();

        this->drawer = addChild<FilterGraph>();
        this->drawer->setScale({ baseWidth, drawerHeight });
        this->drawer->setPosition({0, headerSize});
        
    }

    void FilterNode::draw(SkCanvas *canvas, Float64 deltaTime) {
        Node::draw(canvas, deltaTime);
    }

    FilterNode::FilterNode(UInt inputs, UInt outputs, Float32 socketSize,
                           Float32 baseWidth, Float32 baseHeight, Float32 headerSize,
                           const UserInterface::LinearColor &headerColor) : Node(inputs, outputs, socketSize,
                                                                                 baseWidth, drawerHeight + baseHeight, headerSize,
                                                                                 headerColor) {

        

    }

    void FilterNode::onElementSet() {

        this->filterElement = dynamic_cast<FilterElement*>(getElement());
        this->drawer->filterRef = filterElement->getFilters()[0];
        drawer->addListener(this);
        
    }

    void FilterNode::setPosition(const SkVector &value) {
        Component::setPosition(value);
        drawer->recalculatePath();
    }

    void FilterNode::onValueChange(Float64 frequency, Float64 resonance) {

        auto& filters = filterElement->getFilters();

        for (const auto& filter : filters) {

            filter->setCutoffFrequency(frequency);
            filter->setResonance(resonance);
            
        }
        
    }

}
