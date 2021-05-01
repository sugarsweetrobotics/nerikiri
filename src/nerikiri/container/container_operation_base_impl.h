#pragma once

#include "container_operation_base.h"
#include "../operation/operation_base.h"


using namespace nerikiri;


    /*
class ContainerOperationBase : public OperationBase {

private:
    
public:
    ContainerOperationBase(const std::string& _typeName, const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}) :
        OperationBase(_typeName, operationTypeName, _fullName, defaultArgs) {}
    virtual ~ContainerOperationBase() {}


    virtual Value call(const Value& value) override {
        return Value::error(logger::error("ContainerOperationBase::call called. But this function must be overwritten."));
    }
};
 */