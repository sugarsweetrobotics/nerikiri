#pragma once

#include <string>
#include <nerikiri/object.h>
#include <nerikiri/factory_api.h>

namespace nerikiri {
    class OperationAPI;
    class ConnectionAPI;

    class OperationInletAPI;

    /**
     * 
     * 
     */
    class OperationOutletAPI {
    public:
        virtual ~OperationOutletAPI() {}

    public:

        // virtual OperationAPI* owner() = 0;

        virtual std::string ownerFullName() const = 0;

        virtual Value invokeOwner() = 0;

        virtual Value get() const = 0;

        virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const = 0;

        virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) = 0;

        virtual Value connectTo(const std::shared_ptr<OperationInletAPI>& inlet, const Value& connectionInfo) = 0;

        virtual Value disconnectFrom(const std::shared_ptr<OperationInletAPI>& inlet) = 0;

        virtual Value removeConnection(const std::string& _fullName) = 0;

        virtual Value info() const = 0;

        virtual Value fullInfo() const { return info(); }
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

        virtual bool isNull() const = 0;
        
        virtual std::string name() const = 0;

        virtual std::string ownerFullName() const = 0;

        virtual Value defaultValue() const = 0;

        virtual Value get() const = 0;

        virtual Value put(const Value& value) = 0;

        //virtual OperationAPI* owner() = 0;

        virtual Value executeOwner() = 0;

        virtual Value info() const = 0;

        virtual Value fullInfo() const { return info(); }

        virtual bool isUpdated() const = 0;

        virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const = 0;

        virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) = 0;

        virtual Value connectTo(const std::shared_ptr<OperationOutletAPI>& outlet, const Value& connectionInfo) = 0;

        virtual Value disconnectFrom(const std::shared_ptr<OperationOutletAPI>& outlet) = 0;

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
    
        static constexpr char classNameString[] = "Operation"; 
        static constexpr char nullClassNameString[] = "NullOperation"; 
        static constexpr char factoryClassNameString[] = "OperationFactory"; 
        static constexpr char nullFactoryClassNameString[] = "NullOperationFactory";

        OperationAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}

        virtual ~OperationAPI() {}

    public:
        virtual Value setOwner(const std::shared_ptr<Object>& obj)  {
            return Value::error(logger::error("OperationAPI::setOwner() is not overloaded."));
        }

        virtual const std::shared_ptr<Object> getOwner() const {
            logger::error("OperationAPI::getOwner() is not overloaded.");
            return nullObject<Object>();
        }


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

    template<>
    inline std::shared_ptr<OperationAPI> nullObject() { return nullOperation(); }

    using OperationFactoryAPI = FactoryBase<OperationAPI>;
    
    using NullOperationFactory = NullFactory<OperationAPI>;

    template<>
    inline std::shared_ptr<OperationFactoryAPI> nullObject<OperationFactoryAPI>() { return std::make_shared<NullOperationFactory>(); }

        

}