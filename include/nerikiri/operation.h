#pragma once

#include <memory>
#include <functional>
#include <mutex>
#include <thread>
#include <nerikiri/operation_api.h>
//#include "operation_base.h"

namespace nerikiri {

  std::shared_ptr<OperationAPI> createOperation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(const Value&)>& func = nullptr);

  
}
