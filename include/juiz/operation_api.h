#pragma once

#include <string>
#include <juiz/object.h>
#include <juiz/factory_api.h>

namespace juiz {
    class OperationAPI;
    class ConnectionAPI;

    class InletAPI;

    /**
     * 
     * 
     */
    class OutletAPI {
    public:
        virtual ~OutletAPI() {}

    public:

        // virtual OperationAPI* owner() = 0;

        virtual std::string ownerFullName() const = 0;

        virtual Value invokeOwner() = 0;

        virtual Value get() const = 0;

        virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const = 0;

        //virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) = 0;

        virtual Value connectTo(const std::shared_ptr<InletAPI>& inlet, const Value& connectionInfo) = 0;

        virtual Value disconnectFrom(const std::shared_ptr<InletAPI>& inlet) = 0;

        //virtual Value removeConnection(const std::string& _fullName) = 0;

        virtual Value info() const = 0;

        virtual Value fullInfo() const { return info(); }
    };

    std::shared_ptr<OutletAPI> nullOperationOutlet();
    
    /**
     * 
     * 
     */
    class InletAPI {
    public:
        virtual ~InletAPI() {}
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

        // virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) = 0;

        virtual Value connectTo(const std::shared_ptr<OutletAPI>& outlet, const Value& connectionInfo) = 0;

        virtual Value disconnectFrom(const std::shared_ptr<OutletAPI>& outlet) = 0;

        // virtual Value removeConnection(const std::string& _fullName) = 0;

        virtual std::string fullPath() const {
            return ownerFullName() + ":" + name();
        }
    };

    std::shared_ptr<InletAPI> nullOperationInlet();


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

        virtual std::shared_ptr<OutletAPI> outlet() const = 0;

        virtual std::shared_ptr<InletAPI> inlet(const std::string& name) const = 0;
    
        virtual std::vector<std::shared_ptr<InletAPI>> inlets() const = 0;

        virtual void finalize() override {
            logger::info("OperationAPI(fullName={})::finalize() called.", fullName());
        }
    };

    std::shared_ptr<OperationAPI> nullOperation();

    template<>
    inline std::shared_ptr<OperationAPI> nullObject() { return nullOperation(); }


    template<>
    inline std::shared_ptr<OutletAPI> nullObject() { return nullOperationOutlet(); }
    template<>
    inline std::shared_ptr<InletAPI> nullObject() { return nullOperationInlet(); }

    using OperationFactoryAPI = FactoryBase<OperationAPI>;
    
    using NullOperationFactory = NullFactory<OperationAPI>;

    template<>
    inline std::shared_ptr<OperationFactoryAPI> nullObject<OperationFactoryAPI>() { return std::make_shared<NullOperationFactory>(); }

        

}