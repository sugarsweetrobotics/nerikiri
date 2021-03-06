#include "container_operation_factory_base.h"
using namespace juiz;

class ContainerOperationFactoryBase : public ContainerOperationFactoryAPI {
protected:
    const Value defaultArgs_;
    const std::string operationTypeFullName_;
    const std::string containerTypeFullName_;
public:
  //  virtual std::string containerTypeFullName() const override { return containerTypeFullName_; }
  //  virtual std::string operationTypeFullName() const override { return operationTypeFullName_ ;}
public:
    ContainerOperationFactoryBase(const std::string& className, const std::string& containerTypeFullName, const std::string& operationTypeFullName, const std::string& fullName, const Value& defaultArgs):
      ContainerOperationFactoryAPI(className, juiz::naming::join(containerTypeFullName, operationTypeFullName), juiz::naming::join(containerTypeFullName, fullName)), /*containerTypeFullName_(containerTypeFullName), operationTypeFullName_(operationTypeFullName),*/ defaultArgs_(defaultArgs) {}
    virtual ~ContainerOperationFactoryBase() {}
public:
    virtual std::shared_ptr<Object> create(const std::string& _fullName, const Value& info=Value::error("")) const override {
        return nullOperation();
    }
};


std::shared_ptr<ContainerOperationFactoryAPI> juiz::containerOperationFactoryBase(const std::string& className, const std::string& containerTypeFullName, const std::string& operationTypeFullName, const std::string& fullName, const Value& defaultArgs) {
    return std::make_shared<ContainerOperationFactoryBase>(className, containerTypeFullName, operationTypeFullName, fullName, defaultArgs);
}
