#include "container_operation_factory_base.h"
using namespace nerikiri;

class ContainerOperationFactoryBase : public ContainerOperationFactoryAPI {
protected:
    const Value defaultArgs_;
    const std::string operationTypeFullName_;
    const std::string containerTypeFullName_;
public:
    virtual std::string containerTypeFullName() const override { return containerTypeFullName_; }
    virtual std::string operationTypeFullName() const override { return operationTypeFullName_ ;}
public:
    ContainerOperationFactoryBase(const std::string& className, const std::string& containerTypeFullName, const std::string& operationTypeFullName, const std::string& fullName, const Value& defaultArgs):
    ContainerOperationFactoryAPI(className, nerikiri::naming::join(containerTypeFullName, operationTypeFullName), fullName), containerTypeFullName_(containerTypeFullName), operationTypeFullName_(operationTypeFullName), defaultArgs_(defaultArgs) {}
    virtual ~ContainerOperationFactoryBase() {}
public:
    virtual std::shared_ptr<OperationAPI> create(const std::shared_ptr<ContainerAPI>& container, const std::string& _fullName) const override {
        return nullOperation();
    }
};


std::shared_ptr<ContainerOperationFactoryAPI> nerikiri::containerOperationFactoryBase(const std::string& className, const std::string& containerTypeFullName, const std::string& operationTypeFullName, const std::string& fullName, const Value& defaultArgs) {
    return std::make_shared<ContainerOperationFactoryBase>(className, containerTypeFullName, operationTypeFullName, fullName, defaultArgs);
}
