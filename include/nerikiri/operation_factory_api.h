#pragma once

#include <nerikiri/operation_api.h>
#include <nerikiri/operation.h>


namespace nerikiri {

  class OperationFactoryAPI : public Object {
  public:
    OperationFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName): Object(className, typeName, fullName) {}
    virtual ~OperationFactoryAPI() {}

    virtual std::shared_ptr<OperationAPI> create(const std::string& _fullName) const = 0;
  };


  std::shared_ptr<OperationFactoryAPI> nullOperationFactory();
}