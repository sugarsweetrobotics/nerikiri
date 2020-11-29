#pragma once

#include <string>
#include <nerikiri/object.h>
//#include <nerikiri/operation_inlet_api.h>
//#include <nerikiri/operation_outlet_api.h>

namespace nerikiri {
    class OperationAPI;
    class ConnectionAPI;

    /**
     * 
     * 
     */
    class OperationOutletAPI {
    public:
        virtual ~OperationOutletAPI() {}

    public:

        virtual OperationAPI* owner() = 0;

        virtual Value get() const = 0;

        virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const = 0;

        virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) = 0;

        virtual Value removeConnection(const std::string& _fullName) = 0;

        virtual Value info() const = 0;
    };

    std::shared_ptr<OperationOutletAPI> nullOperationOutlet();
    
    /**
     * 
     * 
     */
    class OperationInletAPI {
    public:
        virtual ~OperationInletAPI() {}
    public:
        virtual std::string name() const = 0;

        virtual Value defaultValue() const = 0;

        virtual Value get() const = 0;

        virtual Value put(const Value& value) = 0;

        virtual OperationAPI* owner() = 0;

        virtual Value info() const = 0;

        virtual bool isUpdated() const = 0;

        virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const = 0;

        virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) = 0;

        virtual Value removeConnection(const std::string& _fullName) = 0;
    };

    std::shared_ptr<OperationInletAPI> nullOperationInlet();

    /**
     * 
     * 
     * 
     */
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

        virtual std::shared_ptr<OperationInletAPI> inlet(const std::string& name) const = 0;
    
        virtual std::vector<std::shared_ptr<OperationInletAPI>> inlets() const = 0;
    };

    std::shared_ptr<OperationAPI> nullOperation();
    

}