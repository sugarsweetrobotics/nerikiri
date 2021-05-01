#pragma once

#include <memory>
#include <functional>
#include <mutex>
#include <thread>
#include <nerikiri/operation_api.h>
#include <nerikiri/operation_factory_api.h>

namespace nerikiri {


  void* operationFactory(const Value& info, std::function<Value(const Value&)>&& func);

  // std::shared_ptr<OperationAPI> createOperation(const std::string& operationTypeName, const std::string& _fullName, const Value& defaultArgs = {}, const std::function<Value(const Value&)>& func = nullptr); 


}
