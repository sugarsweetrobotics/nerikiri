#pragma once

#include <string>
#include <nerikiri/object.h>
//#include <nerikiri/operation_inlet_api.h>
//#include <nerikiri/operation_outlet_api.h>

namespace nerikiri {

    class OperationOutletAPI;
    class OperationInletAPI;

    class OperationAPI : public Object {
    public:

        OperationAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}

        virtual ~OperationAPI() {}

    public:
        virtual Value fullInfo() const = 0;

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

    std::shared_ptr<OperationAPI> nullOperation();
    

}