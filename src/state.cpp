#include "nerikiri/state.h"

using namespace nerikiri;

Value::Value(const int64_t value) : typecode_(VALUE_TYPE_INT), intvalue_(value) {
}
Value::Value(const double value) : typecode_(VALUE_TYPE_DOUBLE), doublevalue_(value) {}
Value::Value(const std::string& value) : typecode_(VALUE_TYPE_STRING), stringvalue_(value) {}
Value::Value(const std::map<std::string, Value>& value) : typecode_(VALUE_TYPE_OBJECT), objectvalue_(value) {}

Value::Value(): typecode_(VALUE_TYPE_NULL){}

Value::Value(Value&& Value): typecode_(Value.typecode_), intvalue_(Value.intvalue_), doublevalue_(Value.doublevalue_), stringvalue_(std::forward<std::string>(Value.stringvalue_)) {
  objectvalue_.merge(Value.objectvalue_);
}

Value::Value(const Value& Value): typecode_(Value.typecode_), intvalue_(Value.intvalue_), doublevalue_(Value.doublevalue_), stringvalue_(Value.stringvalue_), objectvalue_(Value.objectvalue_) {
}

Value::~Value() {}

int64_t Value::intValue() const {
  if (isIntValue()) return intvalue_;
  throw new ValueTypeError(std::string("trying int value acecss. actual ") + getTypeString());
}

double Value::doubleValue() const {
  if (isDoubleValue()) return doublevalue_;
  throw new ValueTypeError(std::string("trying double value acecss. actual ") + getTypeString());
}

const std::string& Value::stringValue() const {
  if (isStringValue()) return stringvalue_;
  throw new ValueTypeError(std::string("trying string value acecss. actual ") + getTypeString());
}
