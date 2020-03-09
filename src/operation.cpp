#include "nerikiri/operation.h"
#include "nerikiri/logger.h"
#include "nerikiri/functional.h"

using namespace nerikiri;
using namespace nerikiri::logger;


Operation::Operation(OperationInfo&& info, std::function<Value(Value)>&& func):
  info_(info),
  function_(func),
  is_null_(false)
   {
  trace("Operation::Operation({})", str(info));
  info_.at("defaultArg").object_for_each([this](const std::string& key, const Value& value) -> void{
      consumerConnectionListDictionary_.emplace(key, ConnectionList());
  });
}

Operation::Operation(const OperationInfo& info, std::function<Value(Value)>&& func):
  info_(info),
  function_(func),
    is_null_(false)
 {
  trace("Operation::Operation({})", str(info));
  info_.at("defaultArg").object_for_each([this](const std::string& key, const Value& value) -> void{
      consumerConnectionListDictionary_.emplace(key, ConnectionList());
  });
}

Operation::~Operation() {
  trace("Operation::~Operation()");
}

Operation Operation::null;

Value nerikiri::call_operation(const Operation& operation, Value&& value) {
  try {
    return operation.function_(value);
  } catch (ValueTypeError& ex) {
    return Value::error(ex.what());
  }
}

