#include "Socket.h"

#include <skia/core/SkPath.h>
#include <skia/core/SkPathEffect.h>
#include <skia/effects/SkDashPathEffect.h>

#include "Application.h"

namespace Flux {
    
    void Socket::initialize() {
        setColor(UserInterface::LinearColor::fromHex(0xffffff55));
    }
    
    void Socket::setPosition(SkVector const& value) {

        Component::setPosition(value);
            
    }

    void Socket::onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) {
            
        if (button == UserInterface::MouseButton::Left) {

            lineEnd = SkVector(f32(x), f32(y));
            dragged = true;

        }

    }

    void Socket::onButtonUp(UserInterface::MouseButton button, Float64 x, Float64 y, Reactive* upTarget) {
            
        if (button == UserInterface::MouseButton::Left) {

            dragged = false;
            
            if(this != upTarget) {
                
                if(auto* socket = dynamic_cast<Socket*>(upTarget)) {

                    if(link(socket)) {

                    
                    }

                    return;
                    
                }

                unlink();
                
            }
            
        }
            
    }

    void Socket::onDrag(UserInterface::MouseButton button, Float64 x, Float64 y) {
            
        if (button == UserInterface::MouseButton::Left) {

            lineEnd = SkVector(f32(x), f32(y));
                
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
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(lineWidth);
        paint.setColor(UserInterface::LinearColor::fromHex(0xffffff55).toSkColor());
        paint.setPathEffect(SkDashPathEffect::Make(ps, 2, f32(linePhase)));
        linePhase -= 20.0 * deltaTime;
        
        const SkVector root = getSocketRootPosition();
        
        if(dragged) {

            const Float32 factor = lineEnd.x() > root.x() ? 0.5f : -0.5f;
            canvas->drawPath(makeSpline(root, lineEnd, factor), paint);
            
        }        
        
        if(isLinked() && flowType == Flow::Input) { return; }
        
        paint.setColor(cableColor.toSkColor());
        
        if(isLinked()) {

            const SkVector end = linkedSocket->getSocketRootPosition();
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

    void Socket::setDataType(DataType value) {

        if(this->type == value) return;
        
        unlink();
        
        this->type = value;

        switch (value) {
        case DataType::Audio:
            setCableColor(UserInterface::LinearColor::fromHex(0x147ff6ff));
            break;
        case DataType::Mod:
            setCableColor(UserInterface::LinearColor::fromHex(0xf61414ff));
            break;
        case DataType::SideChain:
            setCableColor(UserInterface::LinearColor::fromHex(0x8814f6ff));
            break;
        }
        
    }

    void Socket::setCableColor(UserInterface::LinearColor const& value) {

        this->cableColor = value;
        
    }

    void Socket::setFlow(Flow value) {

        if(this->flowType == value) { return; }

        unlink();
        
        this->flowType = value;
        
    }

    bool Socket::link(Socket* socket) {

        if(!canLink(socket)) { return false; }

        if(socket == linkedSocket) { return true; }
        
        unlink();
        socket->unlink();

        this->linkedSocket = socket;
        socket->linkedSocket = this;

        onLink();
        socket->onLink();

        return true;
            
    }
    
    void Socket::unlink() {

        if(isLinked()) {

            linkedSocket->linkedSocket = nullptr;
            linkedSocket->onUnlink();
            
            this->linkedSocket = nullptr;
            this->onUnlink();
            
        }
        
    }

    SkVector Socket::getSocketRootPosition() const {

        SkVector v = getAbsolutePosition();

        if(flowType == Flow::Input) {
            v.fX += getScale().fX;
        }

        v.fY += getScale().fY / 2.0f;

        return v;
        
    }

    bool Socket::canLink(const Socket* socket) const {
        return socket && getParent() != socket->getParent() && socket->flowType != flowType && socket->type == type;
    }

    bool Socket::isLinked() const {
        return linkedSocket;
    }

}
