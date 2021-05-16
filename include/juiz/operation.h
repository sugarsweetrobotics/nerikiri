#pragma once

#include <functional>
#include <juiz/operation_api.h>

namespace juiz {


  void* operationFactory(const Value& info, std::function<Value(const Value&)>&& func);

}
