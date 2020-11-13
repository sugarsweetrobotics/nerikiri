#pragma once

#include <nerikiri/object.h>
#include <nerikiri/operation_api.h>
#include <nerikiri/logger.h>

namespace nerikiri {




    class OperationOutletAPI;
    class OperationInletAPI;

    class ContainerAPI : public Object {
    public:
        ContainerAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}

        virtual ~ContainerAPI() {}

        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const = 0;

        virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const = 0;

        virtual Value addOperation(const std::shared_ptr<OperationAPI>& operation) = 0;

        virtual Value deleteOperation(const std::string& fullName) = 0;
        
    };


    class NullContainer : public ContainerAPI {
    public:

        NullContainer() : ContainerAPI("NullContainer", "NullContainer", "null") {}
        
        virtual ~NullContainer() {}


        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override {
            logger::warn("NullContainer::operations() failed. Container is null.");
            return {};
        }

        virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const override {
            logger::warn("NullContainer::operation(std::string& const) failed. Container is null.");
            return std::make_shared<NullOperation>();
        }
            
        virtual Value addOperation(const std::shared_ptr<OperationAPI>& operation) override {
            return Value::error(logger::warn("NullContainer::{}(std::string& const) failed. Container is null.", __func__));
        }

        virtual Value deleteOperation(const std::string& fullName) override {
            return Value::error(logger::warn("NullContainer::{}(std::string& const) failed. Container is null.", __func__));
        }

    };
}