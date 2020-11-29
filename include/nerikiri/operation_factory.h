#pragma once

#include <nerikiri/operation_factory_api.h>


namespace nerikiri {
  void* operationFactory(const Value& info, std::function<Value(const Value&)>&& func);
}