#pragma once

#include <nerikiri/object.h>
#include <nerikiri/operation_api.h>

#include <nerikiri/ec_state_api.h>

namespace nerikiri {



    class ExecutionContextAPI : public Object {
    private:
        enum ExecutionContextState {
            STARTED,
            STOPPED,
        } state_;
    public:
        ExecutionContextAPI(const std::string& typeName, const std::string& fullName) : Object(typeName, fullName), state_(STOPPED) {}
        
        virtual ~ExecutionContextAPI() {}

        bool isStopped() const { return state_ == STOPPED; }

        bool isStarted() const { return state_ == STARTED; }

        virtual Value start() = 0;

        virtual Value stop() = 0;

        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const = 0;

        virtual Value bind(const std::shared_ptr<OperationAPI>& op) = 0;

        virtual Value unbind(const std::string& fullName) = 0;

        virtual std::shared_ptr<ECStateAPI> startedState() = 0;

        virtual std::shared_ptr<ECStateAPI> stoppedState() = 0;
    };

    std::shared_ptr<ExecutionContextAPI> nullEC();
}