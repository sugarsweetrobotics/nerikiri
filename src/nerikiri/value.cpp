
#include <regex>
#include <sstream>

#include "nerikiri/value.h"



using namespace nerikiri;

Value Value::error(const std::string& msg) {
  return Value{VALUE_TYPE_ERROR, msg};
}

Value Value::list() {
  return Value{std::vector<Value>{}};
}

Value Value::object() {
  return Value{std::map<std::string, Value>{}};
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
  if (isStringValue()) return *stringvalue_;
  if (isError()) throw ValueTypeError(std::string("trying string value acecss. actual Error(") + getErrorMessage() + ")");

  throw ValueTypeError(std::string("trying string value acecss. actual ") + getTypeString());
}

const std::map<std::string, Value>& Value::objectValue() const {
  if (isObjectValue()) return *objectvalue_;
  if (isError()) throw ValueTypeError(std::string("trying object value acecss. actual Error(") + getErrorMessage() + ")");
  throw ValueTypeError(std::string("trying object value acecss. actual ") + getTypeString());
}

const std::vector<Value>& Value::listValue() const {
  if (isListValue()) return *listvalue_;
  if (isError()) throw ValueTypeError(std::string("trying list value acecss. actual Error(") + getErrorMessage() + ")");
  throw ValueTypeError(std::string("trying list value acecss. actual ") + getTypeString());
}



Value nerikiri::merge(const Value& v1, const Value& v2) {
    if((v1.typecode_ == v2.typecode_) && (v1.typecode_ == Value::VALUE_TYPE_LIST)) {
      std::vector<Value> result;
      result.insert(result.end(), v2.listvalue_->begin(), v2.listvalue_->end());
      for(auto& v : *v1.listvalue_) {
        bool match = false;
        for(auto& v2 : *v2.listvalue_) {
          if (v2 == v) match = true;
        }
        if (!match) {
          result.push_back(v);
        }
      }
      return {result};
    } else if ((v1.typecode_ == v2.typecode_) && (v1.typecode_ == Value::VALUE_TYPE_OBJECT)) {
      auto rvalue = Value::object();
      //rvalue.typecode_ = Value::VALUE_TYPE_OBJECT;
      for(auto& [key, value] : *v2.objectvalue_) {
        if (v1.objectvalue_->count(key) > 0) {
          rvalue[key] = merge(v1.at(key), v2.at(key));
        } else {
          rvalue[key] = value;
        }
      }
      for(auto& [key, value] : *v1.objectvalue_) {
        if (v2.objectvalue_->count(key) == 0) {
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
    v.const_list_for_each([&vlist](const auto& iv) {
      iv.const_list_for_each([&vlist](auto& iiv) {
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
        return value.const_list_map<Value>([pattern, substring](auto v) {
        return nerikiri::replaceAll(v, pattern, substring);
        });
    }
    if (value.isObjectValue()) {
        return value.const_object_map<std::pair<std::string,Value>>([pattern, substring](auto key, auto v) {
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