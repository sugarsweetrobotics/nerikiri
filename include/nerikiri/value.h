/**
 * nerikiriで使う基本的なデータ型
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <exception>
#include <sstream>
#include <map>
//#include <optional>
#include <functional>
//#include <algorithm>
#include <cstdlib>
///#include <stdlib.h>
//#include <iostream>

#include <nerikiri/nerikiri.h>
#include <nerikiri/stringutil.h>

namespace nerikiri {

  class ValueTypeError : public std::exception {
  private:
    std::string msg_;
    
  public:
    ValueTypeError(const char* msg) : msg_(std::string("Invalid Value Data Access (") + msg + ")") {}
    ValueTypeError(const std::string& msg) : msg_(std::string("Invalid Value Data Access (") + msg + ")") {}
    
    const char* what() const noexcept {
      return msg_.c_str();
    }
  };


  class Value;


  
  std::string str(const Value& value);
  Value merge(const Value& v1, const Value& v2);

  static std::shared_ptr<Value> errorMessageValue_;
  /**
   *
   *
   */
  class Value {
  private:

    enum VALUE_TYPE_CODE {
			  VALUE_TYPE_NULL,
			  VALUE_TYPE_INT,
        VALUE_TYPE_BOOL,
			  VALUE_TYPE_DOUBLE,
			  VALUE_TYPE_STRING,
			  VALUE_TYPE_OBJECT,
        VALUE_TYPE_LIST,
        VALUE_TYPE_BYTEARRAY,
        VALUE_TYPE_ERROR,
    };
    VALUE_TYPE_CODE typecode_;
  public:
    VALUE_TYPE_CODE getTypeCode() const { return typecode_; }

    std::string getTypeString() const {
      if (isBoolValue()) return "bool";
      else if (isIntValue()) return "int";
      else if (isDoubleValue()) return "double";
      else if (isStringValue()) return "string";
      else if (isObjectValue()) return "object";
      else if (isListValue()) return "list";
      else if (isByteArrayValue()) return "byte";
      else if (isError()) return "error";
      return "null";
    }
  public:
    bool isBoolValue() const { return typecode_ == VALUE_TYPE_BOOL; }

    bool isIntValue() const { return typecode_ == VALUE_TYPE_INT; }
    
    bool isDoubleValue() const { return typecode_ == VALUE_TYPE_DOUBLE; }
    
    bool isStringValue() const { return typecode_ == VALUE_TYPE_STRING; }
    
    bool isObjectValue() const { return typecode_ == VALUE_TYPE_OBJECT; }

    bool isListValue() const { return typecode_ == VALUE_TYPE_LIST; }

    bool isByteArrayValue() const {return typecode_ == VALUE_TYPE_BYTEARRAY; }
    
    bool isNull() const { return typecode_ == VALUE_TYPE_NULL; }

    bool isError() const { return typecode_ == VALUE_TYPE_ERROR; }
  private:

    union {
      bool boolvalue_;
      int64_t intvalue_;
      double doublevalue_;
      std::string* stringvalue_;
      std::vector<Value>* listvalue_;
      std::vector<uint8_t>* bytevalue_;
      std::map<std::string, Value>* objectvalue_;
      //uint32_t bytevaluesize_;
      std::string* errormessage_;
    };



  public:
    /**
     * デフォルトコンストラクタ
     */
    Value() : typecode_(VALUE_TYPE_NULL), stringvalue_(nullptr) {}

    /**
     * デストラクタ
     */
    virtual ~Value() {
      this->_clear();
    }
  private:
    void _clear() {
      if (isStringValue()) {
        delete stringvalue_;
        stringvalue_ = nullptr;
      }
      else if (isListValue()) {
        delete listvalue_; listvalue_ = nullptr;
      }
      else if (isObjectValue()) {
        delete objectvalue_; objectvalue_ = nullptr;
      }
      else if (isError()) {
        delete errormessage_; errormessage_ = nullptr;
      }
      else if (isByteArrayValue()) {
        delete bytevalue_; bytevalue_ = nullptr;
      }
      typecode_ = VALUE_TYPE_NULL;
    }

  public:

    bool boolValue() const;

    int64_t intValue() const;
    
    double doubleValue() const;
    
    const std::string& stringValue() const;

    const std::map<std::string, Value>& objectValue() const;

    const std::vector<Value>& listValue() const;

    const std::vector<uint8_t>& byteArrayValue() const { 
      return *bytevalue_;
    }

    const size_t byteArraySize() const {
      return (*bytevalue_).size();
    }

    std::string getErrorMessage() const { 
      if (isError()) return *errormessage_;
      return "";
    }

  public:

    Value(const VALUE_TYPE_CODE typeCode, const std::string& message) : typecode_(typeCode), errormessage_(new std::string(message)) {}

    explicit Value(const bool value) : typecode_(VALUE_TYPE_BOOL), boolvalue_(value) {}

    Value(const int32_t& value) : typecode_(VALUE_TYPE_INT), intvalue_(value) {}

    Value(int32_t&& value) : typecode_(VALUE_TYPE_INT), intvalue_(std::move(value)) {}

    Value(const int64_t& value) : typecode_(VALUE_TYPE_INT), intvalue_(value) {}

    Value(int64_t&& value)  : typecode_(VALUE_TYPE_INT), intvalue_(std::move(value)) {}

    Value(const double& value) : typecode_(VALUE_TYPE_DOUBLE), doublevalue_(value) {}

    Value(double&& value) : typecode_(VALUE_TYPE_DOUBLE), doublevalue_(std::move(value)) {}

    Value(const std::string& value) : typecode_(VALUE_TYPE_STRING), stringvalue_(new std::string(value)) {}

    Value(std::string&& value) : typecode_(VALUE_TYPE_STRING), stringvalue_(new std::string(std::move(value))) {}

    Value(const char* value) : Value(std::move(std::string(value))) {}

    explicit Value(const std::vector<float>& dbls) : typecode_(VALUE_TYPE_LIST), listvalue_(new std::vector<Value>()) {
      for(auto v : dbls) {
        listvalue_->emplace_back(Value((double)v));
      }
    }

    explicit Value(const std::vector<double>& dbls) : typecode_(VALUE_TYPE_LIST), listvalue_(new std::vector<Value>()) {
      for(auto v : dbls) {
        listvalue_->emplace_back(Value(v));
      }
    }

    explicit Value(const std::vector<bool>& bls) : typecode_(VALUE_TYPE_LIST), listvalue_(new std::vector<Value>()) {
      for(auto v : bls) {
        listvalue_->emplace_back(Value{v});
      }
    }

    Value(const std::vector<Value>& value) : typecode_(VALUE_TYPE_LIST), listvalue_(new std::vector<Value>(value)) {}

    Value(std::vector<Value>& value) : typecode_(VALUE_TYPE_LIST), listvalue_(new std::vector<Value>(std::move(value))) {}

    Value(std::vector<std::pair<std::string, Value>>&& pairs) : typecode_(VALUE_TYPE_OBJECT), objectvalue_(new std::map<std::string, Value>()) {
      for(auto &&p : pairs) {
        objectvalue_->emplace(p.first, std::move(p.second));
      }
    }

    Value(const std::map<std::string, Value>& map) : typecode_(VALUE_TYPE_OBJECT), objectvalue_(new std::map<std::string, Value>(map)) {}

    Value(std::map<std::string, Value>&& map) : typecode_(VALUE_TYPE_OBJECT), objectvalue_(new std::map<std::string, Value>(std::move(map))) {}

    Value(std::initializer_list<std::pair<std::string, Value>>&& vs) : typecode_(VALUE_TYPE_OBJECT), objectvalue_(new std::map<std::string, Value>()) {
      for(auto &p : vs) {
        objectvalue_->emplace(p.first, std::move(p.second));
      }
    }

    explicit Value(const uint8_t* bytes, const uint32_t size) : typecode_(VALUE_TYPE_BYTEARRAY), bytevalue_(new std::vector<uint8_t>()) {
      bytevalue_->resize(size);
      //bytevaluesize_ = size;
      for(int i = 0;i < size;i++) {
        (*bytevalue_)[i] = bytes[i];
      }
    }

    /**
     * コピーコンストラクタ
     */
    Value(const Value& value);

    /**
     * ムーブコンストラクタ
     */
    Value(Value&& value);


  public:
    static Value error(const std::string& msg);
    
    static Value list();

    static Value object();
  private:
    
    
    
  public:
    bool hasKey(const std::string& key) const {
      if (!isObjectValue()) return false;
      return (objectvalue_->count(key) != 0);
    }

    void object_for_each(const std::function<void(const std::string&, Value&)>& func);
    
    void const_object_for_each(const std::function<void(const std::string&, const Value&)>& func) const;

    template<typename T>
    std::vector<T> object_map(const std::function<T(const std::string&, Value&)>& func) {
      std::vector<T> r;
      if (!isObjectValue()) return r;
      for(auto& [k, v] : *objectvalue_) {
        r.emplace_back(func(k, v));
      }
      return r;
    }

    template<typename T>
    std::vector<T> const_object_map(const std::function<T(const std::string&, const Value&)>& func) const {
      if (!isObjectValue()) return {};
      std::vector<T> r;
      for(const auto& [k, v] : *objectvalue_) {
        r.emplace_back(func(k, v));
      }
      return r;
    }

    void list_for_each(const std::function<void(Value&)>& func);

    void const_list_for_each(const std::function<void(const Value&)>& func) const;

    template<typename T>
    std::vector<T> list_map(const std::function<T(Value&)>& func) {
      std::vector<T> r;
      if (!isListValue()) return r;
      for(auto&  v : *listvalue_) {
        r.emplace_back(func(v));
      }
      return r;
    }

    template<typename T>
    std::vector<T> const_list_map(const std::function<T(const Value&)>& func) const {
      std::vector<T> r;
      if (!isListValue()) return r;
      for(auto&  v : *listvalue_) {
        r.push_back(func(v));
      }
      return r;
    }


    const Value& at(const std::string& key) const ;

    Value& emplace(std::pair<std::string, Value>&& v);

    Value& push_back(const Value& str);

    Value& emplace_back(Value&& val);
     
    Value& operator[](const std::string& key) {
      if (typecode_ == VALUE_TYPE_OBJECT) {
        return (*objectvalue_)[key];
      }
      _clear();
      typecode_ = VALUE_TYPE_OBJECT;
      objectvalue_ = new std::map<std::string, Value>();
      return (*objectvalue_)[key];
    }

    const Value& operator[](const std::string& key) const {
      if (!isObjectValue()) {
        std::stringstream ss;
        ss << "Value::operator[string key='" << key << "'] failed. Value is not Object type but "<< this->getTypeString() << ". Value is " << str(*this);
        static auto v = Value::error(ss.str());
        return v;
      //  throw ValueTypeError("Value::operator[std::string] failed. Value is not Object type.");
      }
      return (*objectvalue_)[key];
    }

    Value& operator[](const int key) {
      if (!isListValue()) {
        std::stringstream ss;
        ss << "Value::operator[int key='" << key << "'] failed. Value is not List type but "<< this->getTypeString() << ". Value is " << str(*this);
        static auto v = Value::error(ss.str());
        return v;
        //throw ValueTypeError("Value::operator[int] failed. Value is not List type.");
        //return Value::error("Value::operator[](" + std::to_string(key) + ") failed. Program tried to access as list access. But value is " + getTypeString() + " type.");
      }
      if ((*listvalue_).size() <= key) {
        std::stringstream ss;
        ss << "Value::operator[int key='" << key << "'] failed. Index out of range error. Value is " << str(*this);
        static auto v = Value::error(ss.str());
        return v;
        // throw ValueTypeError("Value::operator[int] failed. Value is List type but size range error.");
        //return Value::error("Value::operator[](" + std::to_string(key) + ") failed. Program tried to access as list access. But list out of bounds.");
      } 
      return (*listvalue_)[key];
    }

    Value& operator=(const Value& value) {
      typecode_ = value.typecode_;
      if (isObjectValue()) {
        objectvalue_ = new std::map<std::string, Value>(*value.objectvalue_);
      }
      else if (isBoolValue()) boolvalue_ = (value.boolvalue_);
      else if (isIntValue()) intvalue_ = (value.intvalue_);
      else if (isDoubleValue()) doublevalue_ = (value.doublevalue_);
      else if (isListValue()) {
        listvalue_ = new std::vector<Value>(*value.listvalue_);
      }
      else if (isStringValue()) {
        stringvalue_ = new std::string(*value.stringvalue_);
      }
      else if (isByteArrayValue()) {
        bytevalue_ = new std::vector<uint8_t>(*value.bytevalue_);
      }
      else if (isError()) {
        errormessage_ = new std::string(*value.errormessage_);
      }
      else {
        typecode_ = VALUE_TYPE_CODE::VALUE_TYPE_ERROR;
        errormessage_ = new std::string("Value::Value(const Value& value) failed. Argument value's typecode is unknown");
      }
      return *this;
    }
  
    Value& operator=(Value&& value) {
      typecode_ = value.typecode_;
      if (isBoolValue()) boolvalue_ = std::move(value.boolvalue_);
      else if (isIntValue()) intvalue_ = std::move(value.intvalue_);
      else if (isDoubleValue()) doublevalue_ = std::move(value.doublevalue_);
      else if (isObjectValue()) objectvalue_ = std::move(value.objectvalue_);
      else if (isListValue()) listvalue_ = std::move(value.listvalue_);
      else if (isStringValue()) stringvalue_ = std::move(value.stringvalue_);
      else if (isByteArrayValue()) bytevalue_ = std::move(value.bytevalue_);
      else if (isError()) errormessage_ = std::move(value.errormessage_);
      else {
        typecode_ = VALUE_TYPE_CODE::VALUE_TYPE_ERROR;
        errormessage_ = new std::string("Value::Value(const Value& value) failed. Argument value's typecode is unknown");
      }
      value.typecode_ = VALUE_TYPE_NULL;
      return *this;
    }

    bool operator==(const Value& v2) const {
      if (typecode_ != v2.typecode_) return false;
      if (isStringValue()) return stringValue() == v2.stringValue();
      else if (isIntValue()) return intValue() == v2.intValue();
      else if (isDoubleValue()) return doubleValue() == v2.doubleValue();
      else if (isBoolValue()) return boolValue() == v2.boolValue();
      else if (isListValue()) {
        if (listvalue_->size() != v2.listvalue_->size()) return false;
        for(size_t i = 0;i < listvalue_->size();i++) {
          if((*listvalue_)[i] != (*v2.listvalue_)[i]) return false;
        }
        return true;
      }
      else if (isObjectValue()) {
        if (objectvalue_->size() != v2.objectvalue_->size()) return false;
        for(const auto& [k, v] : *objectvalue_) {
          if (v != (*v2.objectvalue_).at(k)) return false;
        }
        return true;
      }
      else if (isByteArrayValue()) {
        if (bytevalue_->size() != v2.bytevalue_->size()) return false;
        for(size_t i = 0;i < bytevalue_->size();i++) {
          if(bytevalue_[i] != v2.bytevalue_[i]) return false;
        }
        return true;
      }
      return false;
    }
    
    bool operator!=(const Value& v2) const { return !this->operator==(v2); }

    friend Value merge(const Value& v1, const Value& v2);

#ifdef ERROR_MESSAGE_LEVEL_FAST
#else
    // Value errorMessageValue;
#endif


    static std::string string(const Value& v, const std::string& _default = "") {
      if (v.isError()) return _default;
      if (v.isStringValue()) return v.stringValue();
      return _default;
    }
    static bool boolValue(const Value& v, const bool _default=0) {
      if (v.isError()) return _default;
      if (v.isBoolValue()) return v.boolValue();
      return _default;
    }
    static int64_t intValue(const Value& v, const int64_t _default=0) {
      if (v.isError()) return _default;
      if (v.isIntValue()) return v.intValue();
      return _default;
    }
    static Value merge(const Value& v1, const Value& v2);
  };

/*
  class value_pair : public std::pair<std::string, Value> {
  public:
    value_pair(const char* c, Value&& v): std::pair<std::string, Value>(c, std::move(v)) {}
    value_pair(const char* c, const int64_t v): value_pair(c, Value(v)) {}

  };*/


  inline Value errorValue(const std::string& msg) {
   return Value::error(msg);
  }

Value merge(const Value& v1, const Value& v2);

std::string str(const nerikiri::Value& value);

Value lift(const nerikiri::Value& v);

inline nerikiri::Value replaceAll(const nerikiri::Value& value, const std::string& pattern, const std::string& substring);

#ifdef ERROR_MESSAGE_LEVEL_FAST
    static const Value errorTypeError  { Value::VALUE_TYPE_ERROR, "Value::at() failed. Program tried to access with key value access. But value type is wrong."}; 

    static const Value errorError { Value::VALUE_TYPE_ERROR, "Value::at() failed. Program tried to access with key value access. But value is ERROR type." }; 

    static const Value errorKeyError {Value::VALUE_TYPE_ERROR, "Value::at() failed. Program tried to access with key value access. But key is not included." }; 
#else 

#endif

  

  /**
   * 
   */
  inline std::string getStringValue(const Value& v, const std::string& _default) {
    if (v.isError()) return _default;
    if (v.isStringValue()) return v.stringValue();
    return _default;
  }
  
  /**
   * 
   */
  inline int64_t getIntValue(const Value& v, const int64_t& _default) {
    if (v.isError()) return _default;
    if (v.isIntValue()) return v.intValue();
    return _default;
  }

  inline const Value &getObjectValue(const Value& v, const std::string& key, const Value& defaultVal = {}) {
    auto keys = stringSplit(key, '.');
    if (keys.size() == 0) return defaultVal;
    if (keys.size() == 1) {
      if (v.hasKey(key)) {
        return v.at(key);
      }
    }
    if (v.hasKey(keys[0])) {
      return getObjectValue(v.at(keys[0]), stringJoin(vectorSplice(keys, 1), '.'), defaultVal);
    }
    return defaultVal;
  }

  inline const Value &getListValue(const Value& v, const std::string& key, const Value& defaultVal = Value::list()) {
    auto keys = stringSplit(key, '.');
    if (keys.size() == 0) return defaultVal;
    if (keys.size() == 1) {
      if (v.hasKey(key)) {
        return v.at(key);
      }
    }
    if (v.hasKey(keys[0])) {
      return getObjectValue(v.at(keys[0]), stringJoin(vectorSplice(keys, 1), '.'), defaultVal);
    }
    return defaultVal;
  }

} // namespace nerikiri


