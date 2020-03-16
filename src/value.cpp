#include "nerikiri/value.h"

using namespace nerikiri;

Value::Value(int&& value) : typecode_(VALUE_TYPE_INT), intvalue_(std::move(value)) {}
Value::Value(const int64_t value) : typecode_(VALUE_TYPE_INT), intvalue_(value) {}
Value::Value(const double value) : typecode_(VALUE_TYPE_DOUBLE), doublevalue_(value) {}
Value::Value(const std::string& value) : typecode_(VALUE_TYPE_STRING), stringvalue_(value) {}
Value::Value(std::string&& value) : typecode_(VALUE_TYPE_STRING), stringvalue_(std::move(value)) {}
Value::Value(const std::vector<Value>& value) : typecode_(VALUE_TYPE_LIST), listvalue_(value) {}
Value::Value(const Value& value): typecode_(value.typecode_), intvalue_(value.intvalue_), doublevalue_(value.doublevalue_), stringvalue_(value.stringvalue_), objectvalue_(value.objectvalue_), listvalue_(value.listvalue_) {}

Value::Value(std::vector<std::pair<std::string, Value>>&& ps): typecode_(VALUE_TYPE_OBJECT) {
  for(auto &p : ps) {
    objectvalue_[p.first] = p.second;
  }
}

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

std::string nerikiri::str(const nerikiri::Value& value) {
    if (value.isIntValue()) return std::to_string(value.intValue());
    if (value.isDoubleValue()) return std::to_string(value.doubleValue());
    if (value.isStringValue()) return std::string("\"") + value.stringValue() + "\"";
    if (value.isObjectValue()) {
        std::stringstream ss;
        for(auto [k, v] : value.objectValue()) {
            ss << ",\"" << k << "\":" << str(v);
        }
        ss << "}";
        return ss.str().replace(0, 1, "{");
    }
    if (value.isListValue()) {
        std::stringstream ss;
        for(auto& v : value.listValue()) {
            ss << "," << str(v);
        }
        ss << "]";
        return ss.str().replace(0, 1, "[");
    }
    if (value.isNull()) {
        return "{}";
    }
    if (value.isError()) {
        throw ValueTypeError(value.getErrorMessage());
    }
    return "{\"Error\": \"Value is not supported type.\"}";
}

