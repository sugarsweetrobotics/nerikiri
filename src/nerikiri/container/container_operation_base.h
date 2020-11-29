#pragma once

//#include <nerikiri/container_base.h>
#include "nerikiri/operation_api.h"

namespace nerikiri {
    std::shared_ptr<OperationAPI> containerOperationBase(const std::string& _typeName, const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {});
}