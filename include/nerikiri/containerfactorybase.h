#pragma once

#include <vector>
#include <string>
#include <memory>

#include "nerikiri/value.h"
#include "nerikiri/containerbase.h"
#include "nerikiri/containeroperationfactorybase.h"


namespace nerikiri {

    class ContainerOperationFactoryBase;

    class ContainerFactoryBase {
    private:
      std::vector<std::shared_ptr<ContainerOperationFactoryBase>> operationFactories_;
    public:
      virtual ~ContainerFactoryBase() {}
    public:
      virtual std::string typeName() = 0;
    public:
      virtual std::shared_ptr<ContainerBase> create(const Value& info) = 0;
      
      virtual ContainerFactoryBase& addOperationFactory(std::shared_ptr<ContainerOperationFactoryBase> cof) { 
          operationFactories_.push_back(cof);
          return *this;
      }

      friend class ContainerBase;
    };

}