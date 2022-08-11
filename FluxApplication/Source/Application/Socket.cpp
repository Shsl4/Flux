#include <Application/Socket.h>

#include <skia/core/SkPath.h>
#include <skia/core/SkPathEffect.h>
#include <skia/effects/SkDashPathEffect.h>
#include <skia/effects/SkGradientShader.h>

#include <Flux/Core/Utilities/ArrayUtils.h>
#include <Application/Application.h>
#include <Application/Node.h>

namespace Flux {
    
    void Socket::initialize() {
        setColor(UserInterface::LinearColor::fromHex(0x606060ff));
        this->node = dynamic_cast<Node*>(getParent());
    }
    
    void Socket::setPosition(SkVector const& value) {

        Component::setPosition(value);
            
    }

    void Socket::onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) {
            
        if (button == UserInterface::MouseButton::Left) {

            lineEnd = SkVector::Make(f32(x), f32(y));
            dragged = true;

        }

    }

    void Socket::onButtonUp(UserInterface::MouseButton button, Float64 x, Float64 y, Reactive* upTarget) {
            
        if (button == UserInterface::MouseButton::Left) {

            dragged = false;
            
            if(this != upTarget) {
                
                if(auto* socket = dynamic_cast<Socket*>(upTarget)) {

                    link(socket);
                    return;
                    
                }

                if(flowType == UserInterface::Flow::Input && connections.getSize() > 0) {
                    unlinkAll();
                }
                
            }
            
        }
            
    }

    void Socket::onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {
            
        if (button == UserInterface::MouseButton::Left) {

            lineEnd = SkVector::Make(f32(x), f32(y));
                
        }
            
    }
    
    SkPath Socket::makeSpline(SkVector const& a, SkVector const& b, Float32 factor = 0.5f) {
        
        const SkVector c = { (a.fX + b.fX) / 2.0f, (a.fY + b.fY) / 2.0f };
        
        const Float32 l1 = c.x() - a.x();
        const Float32 x1 = a.x() + l1 * factor;
        const SkVector f1 = { x1, a.y() };

        const Float32 l2 = b.x() - c.x();
        const Float32 x2 = c.x() + l2 * (1.0f - factor);
        const SkVector f2 = { x2, b.y() };
            
        SkPath path;
        path.moveTo(a);
        path.quadTo(f1.x(), f1.y(), c.x(), c.y());
        path.quadTo(f2.x(), f2.y(), b.x(), b.y());
        
        return path;
        
    }
    
    SkPath Socket::makeArc(SkVector const& a, SkVector const& b, Float32 factor = 0.5f) {
        
        const Float32 l1 = b.x() - a.x();
        const Float32 x1 = a.x() + l1 * factor;
        const SkVector f1 = { x1, a.y() };
        
        SkPath path;
        path.moveTo(a);
        path.quadTo(f1.x(), f1.y(), b.x(), b.y());
        
        return path;
        
    }

    SkScalar ps[] = { 10.0f, 10.0f };

    void Socket::draw(SkCanvas* canvas, Float64 deltaTime) {
        
        SkPaint paint;

        const auto col = hasLink() ? node->getHeaderColor().toSkColor() : getColor().darker(10).toSkColor();
        
        paint.setColor(col);
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawCircle(getAbsoluteCenteredPosition(), 5, paint);
        
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(lineWidth);
        paint.setColor(UserInterface::LinearColor::fromHex(0xffffff55).toSkColor());
        paint.setPathEffect(SkDashPathEffect::Make(ps, 2, f32(linePhase)));
        linePhase -= 20.0 * deltaTime;
        
        const SkVector root = getAbsoluteCenteredPosition();
        
        if(dragged) {

            const Float32 factor = lineEnd.x() > root.x() ? 0.5f : -0.5f;
            canvas->drawPath(makeSpline(root, lineEnd, factor), paint);
            
        }        

        if(flowType == UserInterface::Flow::Input) { return; }
        
        paint.setColor(cableColor.toSkColor());
        
        for (const auto* connection : connections) {
            
            const SkVector end = connection->getAbsoluteCenteredPosition();

            SkVector points[2] = { root, end };
            SkColor colors[2] = { node->getHeaderColor().toSkColor().toSkColor(), connection->node->getHeaderColor().toSkColor().toSkColor() };
            
            auto shader = SkGradientShader::MakeLinear(points, colors, nullptr, 2, SkTileMode::kClamp);

            paint.setShader(shader);
            
            const Float32 factor = end.x() > root.x() ? 0.5f : -0.5f;
            canvas->drawPath(makeSpline(root, end, factor), paint);
            
        }

    }

    void Socket::onLink() {

        Console::logDebug("Socket linked");
        
    }

    void Socket::onUnlink() {
        
        Console::logDebug("Socket unlinked");
        
    }

    void Socket::unlinkAll() {

        for (auto* e : connections) {
            unlink(e);
        }
        
    }
    
    void Socket::setDataType(DataType value, const UInt channelIndex) {
        
        unlinkAll();
        
        this->type = value;
        this->channelId = channelIndex;

        switch (value) {
        case DataType::Audio:
            setCableColor(UserInterface::LinearColor::fromHex(0x8814f6ff));
            break;
        case DataType::Mod:
            setCableColor(UserInterface::LinearColor::fromHex(0xf61414ff));
            break;
        case DataType::SideChain:
            setCableColor(UserInterface::LinearColor::fromHex(0x147ff6ff));
            break;
        }
        
    }

    void Socket::setCableColor(UserInterface::LinearColor const& value) {

        this->cableColor = value;
        
    }

    void Socket::setFlow(UserInterface::Flow value) {

        if(this->flowType == value) { return; }

        unlinkAll();
        
        this->flowType = value;
        
    }

    bool Socket::link(Socket* socket) {

        if(!canLink(socket)) { return false; }

        if(ArrayUtils::contains(connections, socket)) { return true; }

        if(flowType == UserInterface::Flow::Input) {

            // Input sockets only accept one input, so unlink everything if trying to link a new socket
            unlinkAll();

            if(!socket->node->getElement()->linkOutput(socket->channelId, channelId, node->getElement())) {
                return false;
            }

        }
        else {

            // If this socket is not an input socket, then the other one necessarily is. Same logic applies.
            socket->unlinkAll();
            
            if(!node->getElement()->linkOutput(channelId, socket->channelId, socket->node->getElement())) {
                return false;
            }
            

        }

        connections += socket;
        socket->connections += this;
        
        onLink();
        socket->onLink();

        return true;
            
    }
    
    void Socket::unlink(Socket* socket) {

        if(socket) {

            // If the socket was linked
            if(ArrayUtils::removeFirstOf(socket->connections, this)) {

                node->getElement()->unlinkInput(channelId);
                ArrayUtils::removeFirstOf(connections, socket);
                socket->onUnlink();
                this->onUnlink();
                
            }
            
        }
        
    }

    SkVector Socket::getSocketRootPosition() const {

        SkVector v = getAbsolutePosition();

        if(flowType == UserInterface::Flow::Input) {
            v.fX += getScale().fX;
        }

        v.fY += getScale().fY / 2.0f;

        return v;
        
    }

    bool Socket::canLink(const Socket* socket) const {
        return socket && socket != this && getParent() != socket->getParent() && socket->flowType != flowType && socket->type == type;
    }

    bool Socket::hasLink() const {
        return connections.getSize() > 0;
    }

    bool Socket::isLinked(const Socket* socket) const {

        try {
            ArrayUtils::firstIndexOf<Socket*>(connections, const_cast<Socket*>(socket));
            ArrayUtils::firstIndexOf(socket->connections, const_cast<Socket*>(this));
            return true;
        }
        catch (std::logic_error const&) {
            return false;
        }
        
    }


}
