#include <Application/Node.h>

namespace Flux {
    
    Node::Node(Audio::PipelineElement* element, const UInt inputs, const UInt outputs, const Float32 socketSize, const Float32 baseWidth,
               const Float32 baseHeight, const Float32 headerSize, const UserInterface::LinearColor& headerColor)
        : inputs(inputs), outputs(outputs), socketSize(socketSize), baseWidth(baseWidth), baseHeight(baseHeight),
          headerSize(headerSize), headerColor(headerColor), element(element) { }

    void Node::initialize() {
            
        const UInt count = inputs > outputs ? inputs : outputs;
        const Float32 ioHeight = socketSize * f32(count);

        setScale( { baseWidth, baseHeight + headerSize + ioHeight } );
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

    SkVector Node::getInputSocketPosition(UInt channel) {

        SkVector e = getAbsolutePosition();
        SkVector v = { socketSize / 2.0f, socketSize / 2.0f + baseHeight + headerSize + socketSize * f32(channel) };
        return e + v;

    }
}
