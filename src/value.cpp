#include "nerikiri/value.h"

using namespace nerikiri;

Value::Value(int&& value) : typecode_(VALUE_TYPE_INT), intvalue_(std::move(value)) {}
Value::Value(const int64_t value) : typecode_(VALUE_TYPE_INT), intvalue_(value) {}
Value::Value(const double value) : typecode_(VALUE_TYPE_DOUBLE), doublevalue_(value) {}
Value::Value(const std::string& value) : typecode_(VALUE_TYPE_STRING), stringvalue_(value) {}
Value::Value(std::string&& value) : typecode_(VALUE_TYPE_STRING), stringvalue_(std::move(value)) {}
//Value::Value(const std::map<std::string, Value>& value) : typecode_(VALUE_TYPE_OBJECT), objectvalue_(value) {}
Value::Value(const std::vector<Value>& value) : typecode_(VALUE_TYPE_LIST), listvalue_(value) {}
Value::Value(const Value& value): typecode_(value.typecode_), intvalue_(value.intvalue_), doublevalue_(value.doublevalue_), stringvalue_(value.stringvalue_), objectvalue_(value.objectvalue_), listvalue_(value.listvalue_) {}
//Value::Value(std::vector<Value>&& value) : typecode_(VALUE_TYPE_LIST), listvalue_(std::move(value)) {}

Value::Value(std::initializer_list<std::pair<std::string, Value>>&& ps) : typecode_(VALUE_TYPE_OBJECT) {
  for(auto &p : ps) {
    objectvalue_[p.first] = p.second;
  }
}

Value::Value(): typecode_(VALUE_TYPE_NULL){}
/*
Value::Value(Value&& value): typecode_(value.typecode_), intvalue_(value.intvalue_), doublevalue_(value.doublevalue_), stringvalue_(std::forward<std::string>(value.stringvalue_)) {
  objectvalue_.merge(value.objectvalue_);
}*/


//Value::Value(std::map<std::string, Value>&& map) : typecode_(VALUE_TYPE_OBJECT), objectvalue_(std::move(map)) {}


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

const std::map<std::string, Value>& Value::objectValue() const {
  if (isObjectValue()) return objectvalue_;
  throw new ValueTypeError(std::string("trying object value acecss. actual ") + getTypeString());
}

const std::vector<Value>& Value::listValue() const {
  if (isListValue()) return listvalue_;
  throw new ValueTypeError(std::string("trying list value acecss. actual ") + getTypeString());
}
