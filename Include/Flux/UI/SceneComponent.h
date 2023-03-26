#pragma once

#include <UI/Component.h>
#include <UI/Animation.h>
#include <UI/Text.h>

namespace Flux {

    class SmoothZoom : public Animation {

    public:

        SmoothZoom(Float32& v, Point& p) : var(v), pVar(p), fromValue(v), fromOffset(p) {
            setDuration(0.2);
        }
        
        void setTargetValue(Float32 target, Point const& p) {

            this->fromValue = Math::easeOut(fromValue, targetValue, f32(progress()));
            this->fromOffset = Math::easeOut(fromOffset, targetOffset, f32(progress()));
            targetValue = target;
            targetOffset = p;
            restart();
   
        }

        void add(Point const& f) {

            targetOffset += f;
            fromOffset += f;

            if(state() != State::Playing) {
                pVar = targetOffset;
            }
            
        }

    protected:
        
        void process(Float64 deltaTime) override {

            var = Math::easeOut(fromValue, targetValue, f32(progress()));
            pVar = Math::easeOut(fromOffset, targetOffset, f32(progress()));

        }

    private:

        Float32& var;
        Point& pVar;
        
        Float32 targetValue = 1.0f;
        Float32 fromValue = 1.0f;
        
        Point targetOffset;
        Point fromOffset;
        
    };
    
    class SceneComponent : public Component, public CursorManager {
        
    public:
        
        SceneComponent(Point const& p, Point const& s);
        
        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

        void initialize() override;

        void handleScroll(Float64 deltaX, Float64 deltaY) override;

        void handleButtonDown(MouseButton button) override;

        void handleDrag(Float64 deltaX, Float64 deltaY) override;

        void scroll(Float64 deltaX, Float64 deltaY) override;

        void doubleClick(MouseButton button, Float64 x, Float64 y) override;

        NODISCARD Component* componentAtPosition(Point const& p) override;

    private:

        NODISCARD Point toWorld(Point const& p) const;
        
        Text* scaleText = nullptr;
        Text* offsetText = nullptr;
        
        Point offset = {};
        Float32 scaling;

        SmoothZoom zoomAnim;
        
    };

}
