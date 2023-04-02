#include <GLFW/glfw3.h>
#include <Utility/Timer.h>

namespace Flux {

    void Timer::single(const Float64 timeInSeconds, Function<void()> const& exec) {
        nTimes(timeInSeconds, 1, exec);
    }

    void Timer::loop(const Float64 timeInSeconds, Function<void()> const& exec) {
        
        nthrowif(!exec, "Trying to start a timer with an invalid callback.");
        nthrowif(timeInSeconds < 0.0, "Trying to start a timer with a negative delay.");
        nthrowif(!terminate, "Trying to start a timer that is already running.");

        this->callback = exec;
        this->delay = timeInSeconds;
        this->maxIterations = 0;
        this->terminate = false;

        this->timerThread = std::thread(&Timer::threadFuncInfinite, this);
        
    }
    
    void Timer::nTimes(const Float64 timeInSeconds, const size_t count, Function<void()> const& exec) {
        
        nthrowif(!exec, "Trying to start a timer with an invalid callback.");
        nthrowif(timeInSeconds < 0.0, "Trying to start a timer with a negative delay.");
        nthrowif(!terminate, "Trying to start a timer that is already running.");
        nthrowif(count == 0, "Trying to start a timer with a count of 0.");

        this->callback = exec;
        this->delay = timeInSeconds;
        this->maxIterations = count;
        this->terminate = false;

        this->timerThread = std::thread(&Timer::threadFunc, this);
        
    }
    
    Timer::~Timer() {
        stop();
        timerThread.join();
    }
    
    void Timer::stop() {
        this->terminate = true;
    }

    void Timer::threadFunc(Timer* timer) {

        size_t currentIteration = 0;
        Float64 lastTime = glfwGetTime();
        
        while(!timer->terminate && currentIteration < timer->maxIterations) {

            const Float64 currentTime = glfwGetTime();

            if(currentTime - lastTime >= timer->delay) {

                timer->callback();
                lastTime = currentTime;
                ++currentIteration;
                
            }
            
        }

        timer->terminate = true;
        
    }

    void Timer::threadFuncInfinite(Timer* timer) {

        Float64 lastTime = glfwGetTime();
        
        while(!timer->terminate) {

            const Float64 currentTime = glfwGetTime();

            if(currentTime - lastTime >= timer->delay) {

                timer->callback();
                lastTime = currentTime;
                
            }
            
        }

        timer->terminate = true;
        
    }
    
}
