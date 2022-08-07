#pragma once

#include <Flux/UI/Components/Component.h>

namespace Flux {

    class Node;
    
    class Socket : public UserInterface::Component {

    public:

        enum class DataType {

            Audio,
            Mod,
            SideChain
            
        };

        enum class Flow {

            Output,
            Input
            
        };
        
        void initialize() override;

        void setPosition(SkVector const& value) override;

        void onButtonDown(UserInterface::MouseButton button, Float64 x, Float64 y) override;

        void onButtonUp(UserInterface::MouseButton button, Float64 x, Float64 y, Reactive* upTarget) override;

        void onDrag(UserInterface::MouseButton button, Float64 x, Float64 y) override;

        static SkPath makeSpline(SkVector const& a, SkVector const& b, Float32 factor);

        static SkPath makeArc(SkVector const& a, SkVector const& b, Float32 factor);

        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void setDataType(DataType value);

        void setFlow(Flow value);

        void onLink();
        
        void onUnlink();

        bool link(Socket* socket);

        void unlink();

        NODISCARD SkVector getSocketRootPosition() const;

        NODISCARD bool canLink(const Socket* socket) const;
        
        NODISCARD bool isLinked() const;

    private:

        void setCableColor(UserInterface::LinearColor const& value);
        
        Socket* linkedSocket = nullptr;

        UserInterface::LinearColor cableColor = UserInterface::LinearColor::fromHex(0x147ff6ff);
        
        DataType type = DataType::Audio;
        Flow flowType = Flow::Output;
        
        bool dragged = false;

        Float32 lineWidth = 2.0;
        Float64 linePhase = 0.0;

        SkVector lineEnd = {};
        
    };
    
}
