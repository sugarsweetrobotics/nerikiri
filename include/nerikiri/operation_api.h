#pragma once


#include "nerikiri/value.h"
#include "nerikiri/logger.h"

namespace nerikiri {

    class OperationAPI {
    public:
        OperationAPI() {}
        virtual ~OperationAPI() {}

    public:

        virtual Value call(const Value& value) = 0;

        virtual Value execute() = 0;
    };

    
    class NullOperation : public OperationAPI, public Object {
    public:
        NullOperation() : Object() {}

        virtual ~NullOperation() {}

        virtual Value call(const Value& value) override { return Value::error(logger::debug("NullOperation::call({}) called.", value)); }
        virtual Value execute() override { return Value::error(logger::debug("NullOperation::error() called.")); }
    };

}