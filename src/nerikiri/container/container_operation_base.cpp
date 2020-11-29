
#include "container_operation_base_impl.h"


std::shared_ptr<OperationAPI> nerikiri::containerOperationBase(const std::string& _typeName, const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs) {
    return std::make_shared<ContainerOperationBase>(_typeName, operationTypeName, _fullName, defaultArgs);
}
