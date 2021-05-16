#pragma once

#include <string>
#include <memory>


#include <juiz/container_api.h>

namespace nerikiri {
  
  std::shared_ptr<ContainerOperationFactoryAPI> containerOperationFactoryBase(const std::string& className, const std::string& containerTypeFullName, const std::string& operationTypeFullName, 
    const std::string& fullName, const Value& defaultArgs);

}