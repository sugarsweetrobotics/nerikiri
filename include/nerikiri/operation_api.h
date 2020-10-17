#pragma once


#include "nerikiri/value.h"
#include "nerikiri/logger.h"

namespace nerikiri {

    class OperationAPI : public Object {
    public:
        OperationAPI() {}

        OperationAPI(const Value& info): Object(info) {}

        virtual ~OperationAPI() {}

    public:

        virtual Value call(const Value& value) = 0;

        virtual Value invoke() = 0;

        virtual Value execute() = 0;
    };

    
    class NullOperation : public OperationAPI {
    public:
        NullOperation() {}

        virtual ~NullOperation() {}

        virtual Value call(const Value& value) override { 
            return Value::error(logger::debug("NullOperation::call({}) called.", value));
        }

        virtual Value invoke() override {
            return Value::error(logger::debug("NullOperation::invoke() called."));
        }

        virtual Value execute() override { 
            return Value::error(logger::debug("NullOperation::error() called.")); 
        }
    };

}