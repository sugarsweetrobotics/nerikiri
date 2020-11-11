#pragma once

//#include <nerikiri/container_base.h>
#include <nerikiri/operation_base.h>

namespace nerikiri {

    class ContainerOperationBase : public OperationBase {
    public:
        ContainerOperationBase(const std::string& _typeName, const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}) :
         OperationBase(_typeName, operationTypeName, _fullName, defaultArgs) {}
        virtual ~ContainerOperationBase() {}
    };

}