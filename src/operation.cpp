#include "nerikiri/operation.h"
#include "nerikiri/logger.h"
#include "nerikiri/functional.h"

using namespace nerikiri;
using namespace nerikiri::logger;

Operation Operation::null;

Value nerikiri::call_operation(const Callable& operation, Value&& value) {
  logger::trace("nerikir::call_operation({})", str(value));
  try {
    return operation.call(std::move(value));
  } catch (ValueTypeError& ex) {
    return Value::error(ex.what());
  }
}

Value nerikiri::invoke_operation(const Invokable& operation) {
  logger::trace("nerikir::invoke_operation()");
  return operation.invoke();
}
  

