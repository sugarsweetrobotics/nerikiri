#pragma once

#include <string>
#include <nerikiri/value.h>
#include <nerikiri/operation_inlet_api.h>
#include <nerikiri/operation_outlet_api.h>

namespace nerikiri {

    class OperationAPI : public Object {
    public:
        OperationAPI() {}

        OperationAPI(const std::string& typeName, const std::string& fullName) : Object(typeName, fullName) {}

        virtual ~OperationAPI() {}

    public:

        virtual std::string operationTypeName() const = 0;

        virtual Value call(const Value& value) = 0;

        /**
         * This function collects the value from inlets and calls the function inside the Operation,
         * but this does not execute the output side operation
         */
        virtual Value invoke() = 0;

        virtual Value execute() = 0;

        virtual std::shared_ptr<OperationOutletAPI> outlet() const = 0;

        virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const = 0;
    };

    
    class NullOperation : public OperationAPI {
    public:
        NullOperation() {}

        virtual ~NullOperation() {}


        virtual std::string operationTypeName() const override { return "NullOperation"; }

        virtual Value call(const Value& value) override { 
            return Value::error(logger::error("NullOperation::{}({}) failed. Operation is null.", __func__, value));
        }

        virtual Value invoke() override { 
            return Value::error(logger::error("NullOperation::{}({}) failed. Operation is null.", __func__));
        }

        virtual Value execute() override { 
            return Value::error(logger::error("NullOperation::{}({}) failed. Operation is null.", __func__));
        }
        
        virtual std::shared_ptr<OperationOutletAPI> outlet() const override { 
            logger::error("NullOperation::{}() failed. Operation is null.", __func__);
            return std::make_shared<NullOperationOutlet>();
        }

        virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const override {
            logger::error("NullOperation::{}() failed. Operation is null.", __func__);
            return {};
        }
    };

}