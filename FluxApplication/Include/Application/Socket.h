#pragma once

#include <Flux/UI/Component.h>
#include <skia/core/SkPath.h>

namespace Flux {

    using namespace UI;
    
    enum class Flow {
        Input,
        Output
    };
    
    enum class DataType {

        Audio,
        Mod,
        SideChain
            
    };
    
    class Socket : public Component {

    public:

        Socket();

        void onButtonDown(MouseButton button, Float64 x, Float64 y) override;

        void onButtonUp(MouseButton button, Float64 x, Float64 y, Reactive* upTarget) override;

        void onDrag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

        static SkPath makeSpline(SkVector const& a, SkVector const& b, Float32 factor);

        static SkPath makeArc(SkVector const& a, SkVector const& b, Float32 factor);

        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void setDataType(DataType value, UInt channelIndex);

        void setFlow(Flow value);

        void onLink();
        
        void onUnlink();

        bool link(Socket* socket);

        void unlink(Socket* socket);
        
        void unlinkAll();

        NODISCARD Point getSocketRootPosition() const;

        NODISCARD bool canLink(const Socket* socket) const;
        
        NODISCARD bool hasLink() const;
        
        NODISCARD bool isLinked(const Socket* socket) const;
    
    protected:
        
        void parentLinked() override;
        
    private:

        void setCableColor(LinearColor const& value);
        
        MutableArray<Socket*> connections = {};

        LinearColor cableColor = LinearColor::fromHex(0x147ff6ff);
        
        DataType type = DataType::Audio;
        Flow flowType = Flow::Output;
        
        bool dragged = false;

        Float32 lineWidth = 2.0;
        Float64 linePhase = 0.0;

        UInt channelId = 0;

        SkVector lineEnd = {};

        class Node* node = nullptr;
        
    };


}
