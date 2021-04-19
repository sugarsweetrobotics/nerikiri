#pragma once

#include <memory>

namespace nerikiri {

    class ExecutionContextAPI {
    private:
    public:
        ExecutionContextAPI() {}
        virtual ~ExecutionContextAPI() {}

        virtual bool onStarting() = 0;
        virtual bool onStarted() = 0;

        virtual bool onStopping() = 0;
        virtual bool onStopped() = 0;

        virtual bool svc() = 0;
    };

    std::shared_ptr<ExecutionContextAPI> nullEC();
}