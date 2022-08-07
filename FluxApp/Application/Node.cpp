#include "Node.h"

namespace Flux {
    
    void Node::initialize() {

        setScale({200, 250});
        setColor(UserInterface::LinearColor::fromHex(0x606060ff));

        auto child = addChild<Socket>();
        child->setScale({ 25, 25 });
        child->setPosition({ -25, getScale().y() -50 });
        child->setFlow(Socket::Flow::Input);
        child->setDataType(Socket::DataType::Audio);
        
        child = addChild<Socket>();
        child->setScale({ 25, 25 });
        child->setPosition({  getScale().fX, getScale().y() - 50 });
        child->setFlow(Socket::Flow::Output);
        child->setDataType(Socket::DataType::Audio);

    }
    
    void Node::draw(SkCanvas* canvas, Float64 deltaTime) {
        
        SkPaint paint;
            
        paint.setColor(getColor().toSkColor());

        const SkVector pos = getAbsolutePosition();
        SkRect rect = SkRect::MakeXYWH(pos.x(), pos.y(), getScale().x(), getScale().y());

        canvas->drawRect(rect, paint);
            
        paint.setColor(UserInterface::LinearColor::fromHex(0x8814f6ff).toSkColor());
        rect = SkRect::MakeXYWH(pos.x(), pos.y(), getScale().x(), 5.0f);
        canvas->drawRect(rect, paint);

        Compound::draw(canvas, deltaTime);
            
    }

    
    void Node::onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) {
            
        if (button == UserInterface::MouseButton::Left) {

            lastClickPos = SkVector(f32(x), f32(y)) - getPosition();
                
        }

    }

    void Node::onDrag(UserInterface::MouseButton button, Float64 x, Float64 y) {
            
        if (button == UserInterface::MouseButton::Left) { setPosition(SkVector(f32(x), f32(y)) - lastClickPos); }
            
    }
    
}
