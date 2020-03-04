#include "nerikiri/operation.h"
#include "nerikiri/logger.h"

using namespace nerikiri;
using namespace nerikiri::logger;


Operation::Operation(OperationInfo&& info, std::function<Value(Value)>&& func):
  info_(info),
  function_(func),
  is_null_(false)
   {
  trace("Operation::Operation({})", str(info));
}

Operation::Operation(const OperationInfo& info, std::function<Value(Value)>&& func):
  info_(info),
  function_(func),
    is_null_(false)
 {
  trace("Operation::Operation({})", str(info));
}

Operation::~Operation() {
  trace("Operation::~Operation()");
}

Operation Operation::null;


Value nerikiri::call_operation(const Operation& operation, Value&& value) {
    return operation.function_(value);
}