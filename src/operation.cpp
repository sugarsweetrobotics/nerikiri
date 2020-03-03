#include "nerikiri/operation.h"
#include "nerikiri/logger.h"

using namespace nerikiri;
using namespace nerikiri::logger;


Operation::Operation(OperationInfo&& info, std::function<ValueMap(ValueMap)>&& func):
  info_(info),
  function_(func) {
  trace("Operation::Operation({})", str(info));
}

Operation::Operation(const OperationInfo& info, std::function<ValueMap(ValueMap)>&& func):
  info_(info),
  function_(func) {
  trace("Operation::Operation({})", str(info));
}

Operation::~Operation() {
  trace("Operation::~Operation()");
}


