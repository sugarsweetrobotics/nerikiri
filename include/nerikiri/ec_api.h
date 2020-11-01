#pragma once

#include <nerikiri/object.h>

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
    };

    class NullExecutionContext : public ExecutionContextAPI {
    public:
        NullExecutionContext() : ExecutionContextAPI("NullExecutionContext", "null") {}
        virtual ~NullExecutionContext() {}

    public:
        virtual Value start() override {
            return Value::error(logger::error("NullExecutionContext::start() called. ExecutionContext is null."));
        }

        virtual Value stop() override {
            return Value::error(logger::error("NullExecutionContext::stop() called. ExecutionContext is null."));
        }

        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override {
            logger::error("NullExecutionContext::operations() called. ExecutionContext is null.");
            return {};
        }

        virtual Value bind(const std::shared_ptr<OperationAPI>& op) override {
            return Value::error(logger::error("NullExecutionContext::{}}() called. ExecutionContext is null.", __func__));
        }

        virtual Value unbind(const std::string& fullName) override {
            return Value::error(logger::error("NullExecutionContext::{}}() called. ExecutionContext is null.", __func__));
        }
    };

}