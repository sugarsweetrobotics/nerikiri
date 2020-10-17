#pragma once

#include "nerikiri/object.h"
#include "nerikiri/operation_api.h"
#include "nerikiri/logger.h"

namespace nerikiri {





    class ContainerAPI : public Object {

    public:

        ContainerAPI() : Object() {}

        ContainerAPI(const Value& info) : Object(info) {}

        virtual ~ContainerAPI() {}

        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const = 0;

        virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const = 0;
        
    };


    class NullContainer : public ContainerAPI {
    public:

        NullContainer() : ContainerAPI() {}
        virtual ~NullContainer() {}


        virtual std::vector<std::shared_ptr<OperationAPI>> operations() const override {
            logger::warn("NullContainer::operations() failed. Container is null.");
            return {};
        }

        virtual std::shared_ptr<OperationAPI> operation(const std::string& fullName) const override {
            logger::warn("NullContainer::operation(std::string& const) failed. Container is null.");
            return std::make_shared<NullOperation>();
        }
    };
}