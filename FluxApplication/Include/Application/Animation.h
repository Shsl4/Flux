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
    
        FORCEINLINE void setDuration(const Float64 value) { this->duration = value; }

        FORCEINLINE void setLooping(const bool value) { this->looping = value; }
        
        FORCEINLINE void setAutoReverse(const bool value) { this->autoReverse = value; }
        
        FORCEINLINE void setPlayDirection(const PlayDirection value) { this->playDirection = value; }
        
        NODISCARD FORCEINLINE bool isLooping() const { return looping; }
    
        NODISCARD FORCEINLINE bool autoReverses() const { return autoReverse; }
    
        NODISCARD FORCEINLINE State getState() const { return state; }
    
        NODISCARD FORCEINLINE PlayDirection getPlayDirection() const { return playDirection; }
    
        NODISCARD FORCEINLINE Float64 getDuration() const { return duration; }
    
        NODISCARD FORCEINLINE Float64 getCurrentTime() const { return currentTime; }
    
        NODISCARD FORCEINLINE Float64 getProgress() const { return currentTime / duration; }

        virtual ~Animation() = default;
    
    protected:
        
        virtual void onPlay() {}
        
        virtual void onPause() {}

        virtual void onStop() {}

        virtual void onRestart() {}

        virtual void onFinish() {}

        virtual void process(Float64 deltaTime) = 0;
    
    private:

        Float64 duration = 1.0;
        bool looping = false;
        bool autoReverse = false;
        Float64 currentTime = 0.0;
        State state = State::Idle;
        PlayDirection playDirection = PlayDirection::Normal;
        
    };

    
}