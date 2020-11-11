#include "nerikiri/logger.h"

#include "nerikiri/connection.h"


using namespace nerikiri;

Value Connection::pull() {
  if (this->isPull()) {
    return outlet_->owner()->invoke();
  }
  return outlet_->get();
}

Value Connection::put(const Value& value) {
  auto v = inlet_->put(value);
  if (this->isPush() || this->isEvent()) {
    return inlet_->owner()->execute();
  }
  return v;
}
