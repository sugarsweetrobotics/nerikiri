
#include "container_operation_base_impl.h"
#include <nerikiri/container_operation_base.h>

using namespace nerikiri;

namespace nerikiri {
    std::shared_ptr<OperationAPI> createOperation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(const Value&)>& func = nullptr); 

    std::shared_ptr<OperationAPI> containerOperationBase(const std::string& _typeName, const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {});
}


Value ContainerOperationBase::setOwner(const std::shared_ptr<Object>& container) { 
    container_ = std::dynamic_pointer_cast<ContainerAPI>(container); 
    if (!container_) {
        return Value::error(logger::error("ContainerOperationBase::setOwner(container) failed. Passed argument (info={}) can not be converted to ContainerAPI pointer.", container->info()));
    }
    auto _typeName = typeName();
    setTypeName(nerikiri::naming::join(container->fullName(), _typeName));
    base_ = createOperation(nerikiri::naming::join(container->fullName(), _typeName), fullName(), defaultArgs_, [this](auto value) {
        return this->call(value);
    });
    return container_->info();
}

/* 
std::shared_ptr<OperationAPI> nerikiri::containerOperationBase(const std::string& _typeName, const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs) {
    return std::make_shared<ContainerOperationBase>(_typeName, operationTypeName, _fullName, defaultArgs);
}
*/
