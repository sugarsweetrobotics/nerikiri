#pragma once

#include <string>
#include <memory>

#include "nerikiri/containeroperationbase.h"

namespace nerikiri {
    

    class ContainerOperationFactoryBase {
    public:
      virtual ~ContainerOperationFactoryBase() {}
    public:
      virtual std::string containerTypeName() = 0;
      virtual std::string typeName() = 0;
    public:
      virtual std::shared_ptr<ContainerOperationBase> create(const Value& info) = 0;
    };

}