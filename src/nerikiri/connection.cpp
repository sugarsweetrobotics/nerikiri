#include "nerikiri/logger.h"

#include "nerikiri/connection.h"


using namespace nerikiri;

Value Connection::pull() {
  if (this->isPull()) {
    return outlet_->invoke();
  }
  return outlet_->get();
}

Value Connection::put(const Value& value) {
  if (this->isPush() || this->isEvent()) {
      return inlet_->execute(value);
  }
  return inlet_->put(value);
}
