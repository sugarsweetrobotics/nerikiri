#pragma once

#include <string>
#include <memory>


#include <nerikiri/container_operation_factory_api.h>

namespace nerikiri {
  
  std::shared_ptr<ContainerOperationFactoryAPI> containerOperationFactoryBase(const std::string& className, const std::string& containerTypeFullName, const std::string& operationTypeFullName, 
    const std::string& fullName, const Value& defaultArgs);

}