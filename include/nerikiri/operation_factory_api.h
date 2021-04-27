#pragma once

#include <nerikiri/operation_api.h>
#include <nerikiri/operation.h>

#include <nerikiri/factory_api.h>

namespace nerikiri {

  class OperationFactoryAPI : public FactoryAPI {
  public:
    OperationFactoryAPI(const std::string& className, const std::string& typeName, const std::string& fullName): FactoryAPI(className, typeName, fullName) {}
    virtual ~OperationFactoryAPI() {}
  };

  std::shared_ptr<OperationFactoryAPI> nullOperationFactory();

  template<>
  inline std::shared_ptr<OperationFactoryAPI> nullObject() { return nullOperationFactory(); }

}