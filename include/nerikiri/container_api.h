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

    std::shared_ptr<ContainerAPI> nullContainer();
}