#pragma once

#include <Nucleus/Nucleus.h>
#include <atomic>
#include <thread>

namespace Flux {

    using namespace Nucleus;

    class Timer final {
    public:

        Timer() = default;

        void single(Float64 timeInSeconds, Function<void()> const& exec);

        void loop(Float64 timeInSeconds, Function<void()> const& exec);

        void nTimes(Float64 timeInSeconds, size_t count, Function<void()> const& exec);

        void stop();

        ~Timer();

    private:

        static void threadFunc(Timer* timer);

        static void threadFuncInfinite(Timer* timer);

        std::atomic<bool> terminate = true;
        size_t maxIterations = false;
        Float64 delay = 0.0;
        Function<void()> callback;
        std::thread timerThread;
    
    };

}
