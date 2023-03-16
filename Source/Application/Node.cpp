#include <Application/Node.h>
#include <Flux/Factory.h>

#include "Application/Socket.h"
#include "Audio/Pipeline/FilterElement.h"

namespace Flux {
    
    Node::Node(const UInt inputs, const UInt outputs, const Float32 socketSize, const Float32 baseWidth,
               const Float32 baseHeight, const Float32 headerSize, const Color& headerColor)
            : Component({ {}, {baseWidth,  baseHeight + headerSize + socketSize * f32(inputs > outputs ? inputs : outputs)} }),
                inputs(inputs), outputs(outputs), socketSize(socketSize), baseWidth(baseWidth), baseHeight(baseHeight),
                    headerSize(headerSize), headerColor(headerColor) {
        
        setColor(Color::fromHex(0x606060ff));
        
    }
    
    void Node::initialize(){
        
        for (UInt i = 0; i < inputs; ++i) {

            const auto child = Factory::create<Socket>();
            addChild(child);
            child->setSize({ socketSize, socketSize });
            child->setPosition({ 0.0f, baseHeight + headerSize + socketSize * f32(i) });
            child->setFlow(Flow::Input);
            child->setDataType(DataType::Audio, i);
            
        }

        for (UInt i = 0; i < outputs; ++i) {

            const auto child = Factory::create<Socket>();
            addChild(child);
            child->setSize({ socketSize, socketSize });
            child->setPosition({ size().x - socketSize, baseHeight + headerSize + socketSize * f32(i) });
            child->setFlow(Flow::Output);
            child->setDataType(DataType::Audio, i);
            
        }
        
    }

    void Node::draw(SkCanvas* canvas, Float64 deltaTime) {

        SkPaint paint;
            
        paint.setColor(color().skColor());

        const Point pos = globalTransform().centeredPosition();
        SkRect rect = SkRect::MakeXYWH(pos.x, pos.y, size().x, size().y);

        canvas->drawRect(rect, paint);
            
        paint.setColor(headerColor.skColor());
        rect = SkRect::MakeXYWH(pos.x, pos.y, size().x, headerSize);
        canvas->drawRect(rect, paint);

        Component::draw(canvas, deltaTime);
            
    }

    void Node::buttonDown(MouseButton button, Float64 x, Float64 y) {
            
        if (button == MouseButton::Left) {

            lastClickPos = Point(f32(x), f32(y)) - position();
                
        }
            
    }

    void Node::drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {

        if (button == MouseButton::Left) { setPosition(Point(f32(x), f32(y)) - lastClickPos); }
            
    }

    Point Node::getOutputSocketPosition(UInt channel) {
        Point e = globalTransform().position;
        Point v = { size().x, baseHeight + headerSize + socketSize * f32(channel) + socketSize / 2.0f};
        return e + v;
    }

    void Node::onElementSet() {}
    
    void Node::willDispose() {
        
        if(element) {
            element->discard();
        }
        
    }

    Point Node::getInputSocketPosition(UInt channel) {

        Point e = globalTransform().position;
        Point v = { socketSize / 2.0f, socketSize / 2.0f + baseHeight + headerSize + socketSize * f32(channel) };
        return e + v;

    }
    

    void FilterNode::draw(SkCanvas *canvas, Float64 deltaTime) {
        Node::draw(canvas, deltaTime);
    }

    void FilterNode::modified() {
        Node::modified();
        drawer->recalculatePath();
    }

    FilterNode::FilterNode(UInt inputs, UInt outputs, Float32 socketSize,
                           Float32 baseWidth, Float32 baseHeight, Float32 headerSize,
                           const Color& headerColor) : Node(inputs, outputs, socketSize,
                                                            baseWidth, drawerHeight + baseHeight, headerSize,
                                                            headerColor) {
        
        
    }
    
    void FilterNode::initialize() {
        
        this->drawer = Factory::create<FilterGraph>(Point(0, headerSize), Point(baseWidth, drawerHeight));
        addChild(this->drawer);
        
    }

    void FilterNode::onElementSet() {

        this->filterElement = dynamic_cast<FilterElement*>(getElement());
        this->drawer->filterRef = filterElement->getFilters()[0];
        drawer->addListener(this);
        
    }
    
    void FilterNode::onValueChange(Float64 frequency, Float64 resonance) {

        auto& filters = filterElement->getFilters();

        for (const auto& filter : filters) {

            filter->setCutoffFrequency(frequency);
            filter->setResonance(resonance);
            
        }
        
    }

}
