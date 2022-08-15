#pragma once

#include <Flux/UI/Components/Component.h>
#include <skia/core/SkPath.h>

namespace Flux {
    
    enum class DataType {

        Audio,
        Mod,
        SideChain
            
    };
    
    class Socket : public UserInterface::Component {

    public:
        
        void initialize() override;

        void setPosition(SkVector const& value) override;

        void onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) override;

        void onButtonUp(UserInterface::MouseButton button, Float64 x, Float64 y, Reactive* upTarget) override;

        void onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

        static SkPath makeSpline(SkVector const& a, SkVector const& b, Float32 factor);

        static SkPath makeArc(SkVector const& a, SkVector const& b, Float32 factor);

        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void setDataType(DataType value, UInt channelIndex);

        void setFlow(UserInterface::Flow value);

        void onLink();
        
        void onUnlink();

        bool link(Socket* socket);

        void unlink(Socket* socket);
        
        void unlinkAll();

        NODISCARD SkVector getSocketRootPosition() const;

        NODISCARD bool canLink(const Socket* socket) const;
        
        NODISCARD bool hasLink() const;
        
        NODISCARD bool isLinked(const Socket* socket) const;
    
    private:

        void setCableColor(UserInterface::LinearColor const& value);
        
        Array<Socket*> connections = {};

        UserInterface::LinearColor cableColor = UserInterface::LinearColor::fromHex(0x147ff6ff);
        
        DataType type = DataType::Audio;
        UserInterface::Flow flowType = UserInterface::Flow::Output;
        
        bool dragged = false;

        Float32 lineWidth = 2.0;
        Float64 linePhase = 0.0;

        UInt channelId = 0;

        SkVector lineEnd = {};

        class Node* node = nullptr;
        
    };


}
