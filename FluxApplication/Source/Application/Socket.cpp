#include <Application/Socket.h>

#include <skia/core/SkPath.h>
#include <skia/core/SkPathEffect.h>
#include <skia/effects/SkDashPathEffect.h>
#include <skia/effects/SkGradientShader.h>

#include <Application/Application.h>
#include <Application/Node.h>

namespace Flux {

    Socket::Socket() {
        setColor(LinearColor::fromHex(0x606060ff));
    }

    void Socket::parentLinked() {
        this->node = dynamic_cast<Node*>(parent());
    }

    void Socket::onButtonDown(MouseButton button, Float64 x, Float64 y) {
            
        if (button == MouseButton::Left) {

            lineEnd = SkVector::Make(f32(x), f32(y));
            dragged = true;

        }

    }

    void Socket::onButtonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget) {
            
        if (button == MouseButton::Left) {

            dragged = false;
            
            if(this != upTarget) {
                
                if(auto* socket = dynamic_cast<Socket*>(upTarget)) {

                    link(socket);
                    return;
                    
                }

                if(flowType == Flow::Input && connections.size() > 0) {
                    unlinkAll();
                }
                
            }
            
        }
            
    }

    void Socket::onDrag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {
            
        if (button == MouseButton::Left) {

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

    void Socket::draw(SkCanvas* canvas, Float64 deltaTime) {
        
        SkPaint paint;

        const auto col = hasLink() ? node->getHeaderColor().skColor() : color().darker(10).skColor();

        const auto root = globalTransform().centeredPosition();
        
        paint.setColor(col);
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawCircle(root.x, root.y, 5, paint);
        
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(lineWidth);
        paint.setColor(LinearColor::fromHex(0xffffff55).skColor());
        SkScalar ps[] = { 10.0f, 10.0f };
        paint.setPathEffect(SkDashPathEffect::Make(ps, 2, f32(linePhase)));
        linePhase -= 20.0 * deltaTime;
        
        if(dragged) {

            const Float32 factor = lineEnd.x() > root.x ? 0.5f : -0.5f;
            canvas->drawPath(makeSpline({ root.x, root.y }, lineEnd, factor), paint);
            
        }        
/*
        if(flowType == Flow::Input) { return; }
        
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
*/
    }

    void Socket::onLink() {

        Console::log("Socket linked\n");
        
    }

    void Socket::onUnlink() {
        
        Console::log("Socket unlinked\n");
        
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
            setCableColor(LinearColor::fromHex(0x8814f6ff));
            break;
        case DataType::Mod:
            setCableColor(LinearColor::fromHex(0xf61414ff));
            break;
        case DataType::SideChain:
            setCableColor(LinearColor::fromHex(0x147ff6ff));
            break;
        }
        
    }

    void Socket::setCableColor(LinearColor const& value) {

        this->cableColor = value;
        
    }

    void Socket::setFlow(Flow value) {

        if(this->flowType == value) { return; }

        unlinkAll();
        
        this->flowType = value;
        
    }

    bool Socket::link(Socket* socket) {

        if(!canLink(socket)) { return false; }

        if(connections.contains(socket)) { return true; }

        if(flowType == Flow::Input) {

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
            if(socket->connections.remove(this)) {

                node->getElement()->unlinkInput(channelId);
                connections.remove(socket);
                socket->onUnlink();
                this->onUnlink();
                
            }
            
        }
        
    }

    Point Socket::getSocketRootPosition() const {

        Point v = globalTransform().centeredPosition();

        if(flowType == Flow::Input) {
            v.x += size().x;
        }

        v.y += size().y / 2.0f;

        return v;
        
    }

    bool Socket::canLink(const Socket* socket) const {
        return socket && socket != this && parent() != socket->parent() && socket->flowType != flowType && socket->type == type;
    }

    bool Socket::hasLink() const {
        return connections.size() > 0;
    }

    bool Socket::isLinked(const Socket* socket) const {

        return connections.contains(const_cast<Socket*>(socket)) &&
            socket->connections.contains(const_cast<Socket*>(this));
        
    }


}
