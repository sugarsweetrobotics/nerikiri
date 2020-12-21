#include "operation_outlet_base.h"

using namespace nerikiri;



Value OperationOutletBase::put(Value&& v) {
  logger::trace("OperaitonOutletBase::put({}) called", v);
  outputBuffer_.push(v);
  for (auto& c : connections_.connections()) {
    logger::trace(" - Connection({}) is detected.", c->fullName());
    c->put(v);
  }
  return std::forward<Value>(v);
}