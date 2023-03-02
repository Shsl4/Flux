#pragma once

#include <Nucleus/Nucleus.h>

using namespace Nucleus;

namespace Flux {

    class Animation {
    
    public:

        enum class State {

            Idle,
            Playing,
            Paused,
            Done
        
        };
        
        enum class PlayDirection {

            Normal,
            Reverse
            
        };

        Animation() = default;
    
        void play();

        void pause();

        void stop();

        void restart();

        void restartFromHere();

        void update(Float64 deltaTime);
        
        void end();
    
        FORCEINLINE void setDuration(const Float64 value) { this->animDuration = value; }

        FORCEINLINE void setLooping(const bool value) { this->loop = value; }
        
        FORCEINLINE void setAutoReverse(const bool value) { this->autoReverse = value; }
        
        FORCEINLINE void setPlayDirection(const PlayDirection value) { this->direction = value; }
        
        NODISCARD FORCEINLINE bool looping() const { return loop; }
    
        NODISCARD FORCEINLINE bool autoReverses() const { return autoReverse; }
    
        NODISCARD FORCEINLINE State state() const { return animState; }
    
        NODISCARD FORCEINLINE PlayDirection playDirection() const { return direction; }
    
        NODISCARD FORCEINLINE Float64 duration() const { return animDuration; }
    
        NODISCARD FORCEINLINE Float64 currentTime() const { return animTime; }
    
        NODISCARD FORCEINLINE Float64 progress() const { return animTime / animDuration; }

        virtual ~Animation() = default;
    
    protected:
        
        virtual void played() {}
        
        virtual void paused() {}

        virtual void stopped() {}

        virtual void restarted() {}

        virtual void finished() {}

        virtual void process(Float64 deltaTime) = 0;
    
    private:

        Float64 animDuration = 1.0;
        bool loop = false;
        bool autoReverse = false;
        Float64 animTime = 0.0;
        State animState = State::Idle;
        PlayDirection direction = PlayDirection::Normal;
        
    };

    
}