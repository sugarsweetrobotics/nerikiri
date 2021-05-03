#pragma once

#include <memory>
#include <functional>
#include <mutex>
#include <thread>
#include <nerikiri/operation_api.h>

namespace nerikiri {


  void* operationFactory(const Value& info, std::function<Value(const Value&)>&& func);

}
