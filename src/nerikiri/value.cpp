
#include <regex>
#include <sstream>

#include <juiz/value.h>
#include <juiz/logger.h>


using namespace nerikiri;


/**
 * コピーコンストラクタ
 */
Value::Value(const Value& value): typecode_(value.typecode_) {
  if (value.isObjectValue()) {
    objectvalue_ = new std::map<std::string, Value>((*value.objectvalue_));
  }
  else if (value.isBoolValue()) boolvalue_ = (value.boolvalue_);
  else if (value.isIntValue()) intvalue_ = (value.intvalue_);
  else if (value.isDoubleValue()) doublevalue_ = (value.doublevalue_);
  else if (value.isListValue()) {
    listvalue_ = new std::vector<Value>(*value.listvalue_);
  }
  else if (value.isStringValue()) {
    stringvalue_ = new std::string(*value.stringvalue_);
  } 
  else if (value.isByteArrayValue()) {
    bytevalue_ = new std::vector<uint8_t>(*value.bytevalue_);
  }
  else if (value.isError()) {
    errormessage_ = new std::string(*value.errormessage_);
  }
  else {
    typecode_ = VALUE_TYPE_CODE::VALUE_TYPE_ERROR;
    errormessage_ = new std::string("Value::Value(const Value& value) failed. Argument value's typecode is unknown");
  }
}

/**
 * ムーブコンストラクタ
 */
Value::Value(Value&& value) : typecode_(std::move(value.typecode_)) {
  if (value.isBoolValue()) boolvalue_ = std::move(value.boolvalue_);
  else if (value.isIntValue()) intvalue_ = std::move(value.intvalue_);
  else if (value.isDoubleValue()) doublevalue_ = std::move(value.doublevalue_);
  else if (value.isObjectValue()) {
    objectvalue_ = std::move(value.objectvalue_);
    value.objectvalue_ = nullptr;
    value.typecode_ = VALUE_TYPE_NULL;
  }
  else if (value.isListValue()) {
    listvalue_ = std::move(value.listvalue_);
    value.listvalue_ = nullptr;
    value.typecode_ = VALUE_TYPE_NULL;
  }
  else if (value.isStringValue()) {
    stringvalue_ = std::move(value.stringvalue_);
    value.stringvalue_ = nullptr;
    value.typecode_ = VALUE_TYPE_NULL;
  }
  else if (value.isByteArrayValue()) {
    bytevalue_ = std::move(value.bytevalue_);
    value.bytevalue_ = nullptr;
    value.typecode_ = VALUE_TYPE_NULL;
  }
  else if (value.isError()) {
    errormessage_ = std::move(value.errormessage_);
    value.errormessage_ = nullptr;
    value.typecode_ = VALUE_TYPE_NULL;
  }
  else {
    typecode_ = VALUE_TYPE_CODE::VALUE_TYPE_ERROR;
    errormessage_ = new std::string("Value::Value(const Value& value) failed. Argument value's typecode is unknown");
    return;
  }

  value.typecode_ = VALUE_TYPE_NULL;
}


Value Value::error(const std::string& msg) {
  return Value{VALUE_TYPE_ERROR, msg};
}

Value Value::list() {
  return Value{std::vector<Value>{}};
}

Value Value::object() {
  return Value{std::map<std::string, Value>{}};
}

Value Value::merge(const Value& v1, const Value& v2) {
  return nerikiri::merge(v1, v2);
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

const Value& Value::at(const std::string& key) const {
  if (isError()) {
    errorMessageValue_ = std::make_shared<Value>(VALUE_TYPE_ERROR,
      "Value::at(\"" + key + "\") failed. Program tried to access with key value access. But value is ERROR type. (ErrorMessage is \"" + this->getErrorMessage() +"\")"
    );
    logger::error(errorMessageValue_->getErrorMessage());
    return *(errorMessageValue_.get());
    //throw ValueTypeError("Value::at(" + key + ") failed. Program tried to access with key value access. But value is ERROR type. (ErrorMessage is " + this->getErrorMessage() +")");
  }
  if (!isObjectValue()) {

    errorMessageValue_ = std::make_shared<Value>(VALUE_TYPE_ERROR,
      "Value::at(\"" + key + "\") failed. Program tried to access with key value access. But value type is " + this->getTypeString()
    );
    logger::error(errorMessageValue_->getErrorMessage());
    return *(errorMessageValue_.get());
    //throw ValueTypeError("Value::at(" + key + ") failed. Program tried to access with key value access. But value type is " + this->getTypeString());
  }
  if (objectvalue_->count(key) == 0) {
    
    errorMessageValue_ = std::make_shared<Value>(VALUE_TYPE_ERROR,
      "Value::at(\"" + key + "\") failed. Program tried to access with key value access. But key (\"" + key + "\") is not included. Value is " + str(*this) + "."
    );
    logger::error(errorMessageValue_->getErrorMessage());
    return *(errorMessageValue_.get());
    //throw ValueTypeError("Value::at(" + key + ") failed. Program tried to access with key value access. But key (" + key + ") is not included.");
  }
  return objectvalue_->at(key);
}


Value& Value::emplace(std::pair<std::string, Value>&& v) {
  if (typecode_ == VALUE_TYPE_OBJECT) {
//      listvalue_->clear();
//      bytevalue_->clear();
    objectvalue_->emplace(std::move(v));
  } else {
    _clear();
    this->typecode_ = VALUE_TYPE_ERROR;
    errormessage_ = new std::string("Value::emplace(std::pair<std::string, Value>&&) failed.");
  }
  return *this;
}

Value& Value::push_back(const Value& str) {
  if (typecode_ == VALUE_TYPE_LIST) {
    listvalue_->push_back(str);
  } else {
    _clear();
    this->typecode_ = VALUE_TYPE_ERROR;
    errormessage_ = new std::string("Value::push_back(const Value&) failed.");
  }
  return *this;
}

Value& Value::emplace_back(Value&& val) {
  if (typecode_ == VALUE_TYPE_LIST) {
    listvalue_->emplace_back(str);
  } else {
    _clear();
    this->typecode_ = VALUE_TYPE_ERROR;
    errormessage_ = new std::string("Value::emplace_back(const Value&) failed.");
  }
  return *this;
}
  
void Value::list_for_each(const std::function<void(Value&)>& func) {
  if (!isListValue()) return;
  for(auto& v: *listvalue_) {
      func(v);
  }
}

void Value::const_list_for_each(const std::function<void(const Value&)>& func) const {
  if (!isListValue()) return;
  for(const auto& v: *listvalue_) {
      func(v);
  }
}

void Value::object_for_each(const std::function<void(const std::string&, Value&)>& func) {
  if (!isObjectValue()) return;
  for(auto& [k, v] : *objectvalue_) {
      func(k, v);
  }
}

void Value::const_object_for_each(const std::function<void(const std::string&, const Value&)>& func) const {
  if (!isObjectValue()) return;
  for(const auto& [k, v] : *objectvalue_) {
      func(k, v);
  }
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
  if (value.isBoolValue()) {
    return value.boolValue() ? "true" : "false";
  }
  if (value.isError()) {
  return "{\"Error\": \"Value is error('" + value.getErrorMessage() + "').\"}";
      //throw ValueTypeError(value.getErrorMessage());
  }
  std::stringstream ss;

  ss << "{\"Error\": \"nerikiri::str(Value&) function Error. Value is not supported type. Type code is " << (int32_t)value.getTypeCode() << "\"}";
  return ss.str();
}
