#include "nerikiri/state.h"

using namespace nerikiri;

State::State(const int64_t value) : typecode_(STATE_TYPE_INT), intvalue_(value) {
}
State::State(const double value) : typecode_(STATE_TYPE_DOUBLE), doublevalue_(value) {}
State::State(const std::string& value) : typecode_(STATE_TYPE_STRING), stringvalue_(value) {}
State::State(const std::map<std::string, State>& value) : typecode_(STATE_TYPE_OBJECT), objectvalue_(value) {}

State::State(): typecode_(STATE_TYPE_NULL){}

State::State(State&& state): typecode_(state.typecode_), intvalue_(state.intvalue_), doublevalue_(state.doublevalue_), stringvalue_(std::forward<std::string>(state.stringvalue_)) {
  objectvalue_.merge(state.objectvalue_);
}

State::State(const State& state): typecode_(state.typecode_), intvalue_(state.intvalue_), doublevalue_(state.doublevalue_), stringvalue_(state.stringvalue_), objectvalue_(state.objectvalue_) {
}

State::~State() {}

int64_t State::intValue() const {
  if (isIntValue()) return intvalue_;
  throw new StateTypeError(std::string("trying int value acecss. actual ") + getTypeString());
}

double State::doubleValue() const {
  if (isDoubleValue()) return doublevalue_;
  throw new StateTypeError(std::string("trying double value acecss. actual ") + getTypeString());
}

const std::string& State::stringValue() const {
  if (isStringValue()) return stringvalue_;
  throw new StateTypeError(std::string("trying string value acecss. actual ") + getTypeString());
}
