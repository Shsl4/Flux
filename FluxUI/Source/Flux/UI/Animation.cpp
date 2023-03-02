#include <Flux/UI/Animation.h>

namespace Flux {

    void Animation::play() {

        if(animState == State::Done || animState == State::Playing) return;
    
        this->animState = State::Playing;

        played();
    
    }

    void Animation::pause() {

        this->animState = State::Paused;

        paused();
    
    }

    void Animation::stop() {

        this->animState = State::Idle;
        this->animTime = 0.0;

        stopped();
    
    }

    void Animation::restart() {

        this->animState = State::Playing;

        if (direction == PlayDirection::Reverse) {
            this->animTime = this->animDuration;
        }
        else {
            this->animTime = 0.0;
        }

        restarted();
    
    }

    void Animation::restartFromHere() {

        this->animState = State::Playing;
        restarted();
        
    }

    void Animation::update(const Float64 deltaTime) {

        if(animState != State::Playing) return;
        
        process(deltaTime);
    
        switch (direction) {
        
        case PlayDirection::Normal:

            animTime = Math::clamp(animTime + deltaTime, 0.0, animDuration);
        
            if(Math::deq(animTime, animDuration)) {

                finished();
            
                if(loop) {

                    if(autoReverse) {
                        direction = PlayDirection::Reverse;
                    }
                    else {
                        animTime = 0.0;
                    }
                
                }
                else {
                    animState = State::Done;
                }
            
            }
        
            break;
        
        case PlayDirection::Reverse:

            animTime = Math::clamp(animTime - deltaTime, 0.0, animDuration);
        
            if(Math::deq(animTime, 0.0)) {

                finished();
            
                if(loop) {

                    if(autoReverse) {
                        direction = PlayDirection::Normal;
                    }
                    else {
                        animTime = animDuration;
                    }
                
                }
                else {
                    animState = State::Done;
                }
            
            }
            
            break;
        
        }
    }

    void Animation::end() {

        animTime = animDuration;
    
    }
    
}