#include <Application/Animation.h>

namespace Flux {

    void Animation::play() {

        if(state == State::Done || state == State::Playing) return;
    
        this->state = State::Playing;

        onPlay();
    
    }

    void Animation::pause() {

        this->state = State::Paused;

        onPause();
    
    }

    void Animation::stop() {

        this->state = State::Idle;
        this->currentTime = 0.0;

        onStop();
    
    }

    void Animation::restart() {

        this->state = State::Playing;

        if (playDirection == PlayDirection::Reverse) {
            this->currentTime = this->duration;
        }
        else {
            this->currentTime = 0.0;
        }

        onRestart();
    
    }

    void Animation::restartFromHere() {

        this->state = State::Playing;
        onRestart();
        
    }

    void Animation::update(const Float64 deltaTime) {

        if(state != State::Playing) return;
        
        process(deltaTime);
    
        switch (playDirection) {
        
        case PlayDirection::Normal:
        
            currentTime = Math::clamp(currentTime + deltaTime, 0.0, duration);
        
            if(Math::deq(currentTime, duration)) {

                onFinish();
            
                if(looping) {

                    if(autoReverse) {
                        playDirection = PlayDirection::Reverse;
                    }
                    else {
                        currentTime = 0.0;
                    }
                
                }
                else {
                    state = State::Done;                
                }
            
            }
        
            break;
        
        case PlayDirection::Reverse:

            currentTime = Math::clamp(currentTime - deltaTime, 0.0, duration);
        
            if(Math::deq(currentTime, 0.0)) {

                onFinish();
            
                if(looping) {

                    if(autoReverse) {
                        playDirection = PlayDirection::Normal;
                    }
                    else {
                        currentTime = duration;
                    }
                
                }
                else {
                    state = State::Done;                
                }
            
            }
            
            break;
        
        }
    }

    void Animation::end() {

        currentTime = duration;
    
    }
    
}