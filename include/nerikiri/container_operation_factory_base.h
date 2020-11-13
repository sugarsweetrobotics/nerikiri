#pragma once

#include <string>
#include <memory>


#include <nerikiri/container_operation_factory_api.h>

namespace nerikiri {
  
    class ContainerOperationFactoryBase : public ContainerOperationFactoryAPI {
    protected:
      const Value defaultArgs_;
      const std::string operationTypeFullName_;
      const std::string containerTypeFullName_;
    public:
      virtual std::string containerTypeFullName() const { return containerTypeFullName_; }
      virtual std::string operationTypeFullName() const { return operationTypeFullName_ ;}
    public:
      ContainerOperationFactoryBase(const std::string& className, const std::string& containerTypeFullName, const std::string& operationTypeFullName, const std::string& fullName, const Value& defaultArgs):
        ContainerOperationFactoryAPI(className, nerikiri::naming::join(containerTypeFullName, operationTypeFullName), fullName), containerTypeFullName_(containerTypeFullName), operationTypeFullName_(operationTypeFullName), defaultArgs_(defaultArgs) {}
      virtual ~ContainerOperationFactoryBase() {}
    public:
      //virtual std::shared_ptr<ContainerOperationBase> create(const std::shared_ptr<ContainerBase>& container, const std::string& fullName) = 0;
    };

}