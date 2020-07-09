
#include <regex>
#include <sstream>

#include "nerikiri/value.h"



using namespace nerikiri;

Value Value::error(const std::string& msg) {
    Value v;
    v.typecode_ = VALUE_TYPE_ERROR;
    v.errormessage_ = msg;
    return v;
}

Value Value::list() {
    Value v;
    v.typecode_ = VALUE_TYPE_LIST;
    return v;
}

Value Value::object() {
    Value v;
    v.typecode_ = VALUE_TYPE_OBJECT;
    return v;
}

//inline Value::Value(bool&& value) : typecode_(VALUE_TYPE_BOOL), boolvalue_(std::move(value)) {}
Value::Value(int&& value) : typecode_(VALUE_TYPE_INT), intvalue_(std::move(value)) {}

Value::Value(const bool value) : typecode_(VALUE_TYPE_BOOL), boolvalue_(value) {}

Value::Value(const int64_t value) : typecode_(VALUE_TYPE_INT), intvalue_(value) {}

Value::Value(const double value) : typecode_(VALUE_TYPE_DOUBLE), doublevalue_(value) {}

Value::Value(const std::string& value) : typecode_(VALUE_TYPE_STRING), stringvalue_(value) {}

Value::Value(std::string&& value) : typecode_(VALUE_TYPE_STRING), stringvalue_(std::move(value)) {}

Value::Value(const std::vector<Value>& value) : typecode_(VALUE_TYPE_LIST), listvalue_(value) {}

Value::Value(const Value& value): typecode_(value.typecode_), boolvalue_(value.boolvalue_), intvalue_(value.intvalue_), doublevalue_(value.doublevalue_), stringvalue_(value.stringvalue_), objectvalue_(value.objectvalue_), listvalue_(value.listvalue_), bytevalue_(value.bytevalue_) {}

Value::Value(const std::vector<float>& dbls) : typecode_(VALUE_TYPE_LIST) {
  for(auto v : dbls) {
    listvalue_.emplace_back(Value((double)v));
  }
}

Value::Value(const VALUE_TYPE_CODE typeCode, const std::string& message) {
  typecode_ = typeCode;
  errormessage_ = message;
}

Value::Value(const std::vector<double>& dbls) : typecode_(VALUE_TYPE_LIST) {
  for(auto v : dbls) {
    listvalue_.emplace_back(Value(v));
  }
}

Value::Value(const std::vector<bool>& bls) : typecode_(VALUE_TYPE_LIST) {
  for(auto v : bls) {
    Value val{v};
    listvalue_.emplace_back(val);
  }
}

Value::Value(const uint8_t* bytes, const uint32_t size) : typecode_(VALUE_TYPE_BYTEARRAY) {
  //std::cout << "value::byte(" << size << ")" << std::endl;
  bytevalue_.resize(size);
  bytevaluesize_ = size;
  //memcpy(&bytevalue_[0], bytes, size);
  for(int i = 0;i < size;i++) {
    bytevalue_[i] = bytes[i];
  }
  //std::cout << "value::byte OK" << std::endl;
}

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

Value::Value(Value&& value) : typecode_(std::move(value.typecode_)), 
    boolvalue_(std::move(value.boolvalue_)),
    intvalue_(std::move(value.intvalue_)),
    doublevalue_(std::move(value.doublevalue_)),
    objectvalue_(std::move(value.objectvalue_)),
    listvalue_(std::move(value.listvalue_)),
    stringvalue_(std::move(value.stringvalue_)),
    bytevalue_(std::move(value.bytevalue_)) {
}

Value::~Value() {}



std::string Value::getTypeString() const {
    if (isIntValue()) return "int";
    else if (isDoubleValue()) return "double";
    else if (isStringValue()) return "string";
    else if (isObjectValue()) return "object";
    else if (isListValue()) return "list";
    else if (isBoolValue()) return "bool";
    else if (isError()) return "error";
    return "null";
}


void Value::object_for_each(std::function<void(const std::string&, Value&)>&& func) {
    if (!isObjectValue()) return;
    for(auto& [k, v] : objectvalue_) {
        func(k, v);
    }
}

void Value::object_for_each(std::function<void(const std::string&, const Value&)>&& func) const {
    if (!isObjectValue()) return;
    for(const auto& [k, v] : objectvalue_) {
        func(k, v);
    }
}
void Value::list_for_each(std::function<void(const Value&)>&& func) const {
    if (!isListValue()) return;
    for(auto& v: listvalue_) {
        func(v);
    }
}


bool Value::boolValue() const {
  if (isBoolValue()) return boolvalue_;
  if (isError()) throw ValueTypeError(std::string("trying bool value acecss. actual Error(") + getErrorMessage() + ")");
  throw ValueTypeError(std::string("trying bool value acecss. actual ") + getTypeString());
}

int64_t Value::intValue() const {
  if (isIntValue()) return intvalue_;
  if (isError()) throw ValueTypeError(std::string("trying int64 value acecss. actual Error(") + getErrorMessage() + ")");
  throw ValueTypeError(std::string("trying int value acecss. actual ") + getTypeString());
}

double Value::doubleValue() const {
  if (isDoubleValue()) return doublevalue_;
  if (isError()) throw ValueTypeError(std::string("trying double value acecss. actual Error(") + getErrorMessage() + ")");
  throw ValueTypeError(std::string("trying double value acecss. actual ") + getTypeString());
}

const std::string& Value::stringValue() const {
  if (isStringValue()) return stringvalue_;
  if (isError()) throw ValueTypeError(std::string("trying string value acecss. actual Error(") + getErrorMessage() + ")");

  throw ValueTypeError(std::string("trying string value acecss. actual ") + getTypeString());
}

const std::map<std::string, Value>& Value::objectValue() const {
  if (isObjectValue()) return objectvalue_;
  if (isError()) throw ValueTypeError(std::string("trying object value acecss. actual Error(") + getErrorMessage() + ")");
  throw ValueTypeError(std::string("trying object value acecss. actual ") + getTypeString());
}

const std::vector<Value>& Value::listValue() const {
  if (isListValue()) return listvalue_;
  if (isError()) throw ValueTypeError(std::string("trying list value acecss. actual Error(") + getErrorMessage() + ")");
  throw ValueTypeError(std::string("trying list value acecss. actual ") + getTypeString());
}



Value nerikiri::merge(const Value& v1, const Value& v2) {
    if((v1.typecode_ == v2.typecode_) && (v1.typecode_ == Value::VALUE_TYPE_LIST)) {
      std::vector<Value> result;
      result.insert(result.end(), v2.listvalue_.begin(), v2.listvalue_.end());
      for(auto& v : v1.listvalue_) {
        bool match = false;
        for(auto& v2 : v2.listvalue_) {
          if (v2 == v) match = true;
        }
        if (!match) {
          result.push_back(v);
        }
      }
      return {result};
    } else if ((v1.typecode_ == v2.typecode_) && (v1.typecode_ == Value::VALUE_TYPE_OBJECT)) {
      Value rvalue;
      rvalue.typecode_ = Value::VALUE_TYPE_OBJECT;
      for(auto& [key, value] : v2.objectvalue_) {
        if (v1.objectvalue_.count(key) > 0) {
          rvalue[key] = merge(v1.at(key), v2.at(key));
        } else {
          rvalue[key] = value;
        }
      }
      for(auto& [key, value] : v1.objectvalue_) {
        if (v2.objectvalue_.count(key) == 0) {
          rvalue[key] = value;
        }
      }
      return rvalue;
    }
    return v2;
}


nerikiri::Value nerikiri::lift(const nerikiri::Value& v) {
    if (v.isError()) return v;
    if (!v.isListValue()) return v;
    if (v.listValue().size() == 0) return v;
    if (!v.listValue()[0].isListValue()) return v;

    std::vector<Value> vlist;
    v.list_for_each([&vlist](auto& iv) {
    iv.list_for_each([&vlist](auto& iiv) {
        vlist.push_back(iiv);      
    });
    });
    return vlist;
}

Value nerikiri::replaceAll(const nerikiri::Value& value, const std::string& pattern, const std::string& substring) {
    if (value.isStringValue()) {
        return std::regex_replace(value.stringValue(), std::regex(pattern.c_str()), substring);
    }
    if (value.isListValue()) {
        return value.list_map<Value>([pattern, substring](auto v) {
        return nerikiri::replaceAll(v, pattern, substring);
        });
    }
    if (value.isObjectValue()) {
        return value.object_map<std::pair<std::string,Value>>([pattern, substring](auto key, auto v) {
        return std::make_pair(key, nerikiri::replaceAll(v, pattern, substring));
        });
    }
    return value;
}

std::string nerikiri::str(const nerikiri::Value& value) {
    if (value.isIntValue()) return std::to_string(value.intValue());
    if (value.isDoubleValue()) return std::to_string(value.doubleValue());
    if (value.isStringValue()) return std::string("\"") + value.stringValue() + "\"";
    if (value.isObjectValue()) {
      if (value.objectValue().size() == 0) return "{}";
        std::stringstream ss;
        for(auto [k, v] : value.objectValue()) {
            ss << ",\"" << k << "\":" << str(v);
        }
        ss << "}";
        return ss.str().replace(0, 1, "{");
    }
    if (value.isListValue()) {
      if (value.listValue().size() == 0) return "[]";
        std::stringstream ss;
        for(auto& v : value.listValue()) {
            ss << "," << str(v);
        }
        ss << "]";
        return ss.str().replace(0, 1, "[");
    }
    if (value.isByteArrayValue()) {
      return "[\"bytes\"]";
    }
    if (value.isNull()) {
        return "{}";
    }
    if (value.isError()) {
    return "{\"Error\": \"Value is error('" + value.getErrorMessage() + "').\"}";
        //throw ValueTypeError(value.getErrorMessage());
    }
    return "{\"Error\": \"Value is not supported type.\"}";
}