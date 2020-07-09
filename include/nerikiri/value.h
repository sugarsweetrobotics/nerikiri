/**
 * nerikiriで使う基本的なデータ型
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <exception>
#include <map>
//#include <optional>
#include <functional>
//#include <algorithm>
#include <cstdlib>
///#include <stdlib.h>
//#include <iostream>

#include "nerikiri/nerikiri.h"

namespace nerikiri {

  class ValueTypeError : public std::exception {
  private:
    std::string msg_;
    
  public:
    ValueTypeError(const char* msg) : msg_(msg) {}
    ValueTypeError(const std::string& msg) : msg_(msg) {}
    
    const char* what() const noexcept {
      return (std::string("Invalid Value Data Access (") + msg_ + ")").c_str();
    }
  };


  class Value;


  
  std::string str(const Value& value);
  Value merge(const Value& v1, const Value& v2);

  /**
   *
   *
   */
  class Value {

  public:
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
    
  private:
    VALUE_TYPE_CODE typecode_;
    
  private:
    bool boolvalue_;
    int64_t intvalue_;
    double doublevalue_;
    std::string stringvalue_;
    std::map<std::string, Value> objectvalue_;
    std::vector<Value> listvalue_;
    std::vector<uint8_t> bytevalue_;
    uint32_t bytevaluesize_;
    std::string errormessage_;
  public:
    Value();

    explicit Value(const bool value);
    //explicit Value(bool&& value);
    Value(const int64_t value);
    Value(int&& value);
    Value(const double value);
    Value(const std::string& value);
    Value(const char* value) : Value(std::string(value)) {}
    Value(std::string&& value);
    Value(const Value& Value);
    Value(const std::vector<Value>& value);
    Value(std::vector<std::pair<std::string, Value>>&& value);
    Value(std::initializer_list<std::pair<std::string, Value>>&& vs);
    Value(const VALUE_TYPE_CODE typeCode, const std::string& message);
    Value(Value&& value);

    explicit Value(const std::vector<float>& dbls);

    explicit Value(const std::vector<double>& dbls);

    explicit Value(const std::vector<bool>& bls);

    explicit Value(const uint8_t* byte, const uint32_t size);

    virtual ~Value();

  public:
    static Value error(const std::string& msg);
    
    static Value list();

    static Value object();
  private:
    
    void _clear() {
      typecode_ = VALUE_TYPE_NULL;
    }
    
  public:
    std::string getTypeString() const;

    bool isBoolValue() const { return typecode_ == VALUE_TYPE_BOOL; }

    bool isIntValue() const { return typecode_ == VALUE_TYPE_INT; }
    
    bool isDoubleValue() const { return typecode_ == VALUE_TYPE_DOUBLE; }
    
    bool isStringValue() const { return typecode_ == VALUE_TYPE_STRING; }
    
    bool isObjectValue() const { return typecode_ == VALUE_TYPE_OBJECT; }

    bool isListValue() const { return typecode_ == VALUE_TYPE_LIST; }

    bool isByteArrayValue() const {return typecode_ == VALUE_TYPE_BYTEARRAY; }
    
    bool isNull() const { return typecode_ == VALUE_TYPE_NULL; }

    bool isError() const { return typecode_ == VALUE_TYPE_ERROR; }

    std::string getErrorMessage() const { return errormessage_; }
    
    bool hasKey(const std::string& key) const {
      if (!isObjectValue()) return false;
      return (objectvalue_.count(key) != 0);
    }

    void object_for_each(std::function<void(const std::string&, Value&)>&& func);
    
    void object_for_each(std::function<void(const std::string&, const Value&)>&& func) const;

    template<typename T>
    std::vector<T> object_map(std::function<T(const std::string&, Value&)>&& func) {
      std::vector<T> r;
      if (!isObjectValue()) return r;
      for(auto& [k, v] : objectvalue_) {
        r.emplace_back(func(k, v));
      }
      return r;
    }

    template<typename T>
    std::vector<T> object_map(std::function<T(const std::string&, const Value&)>&& func) const {
      if (!isObjectValue()) return {};
      std::vector<T> r;
      for(const auto& [k, v] : objectvalue_) {
        r.emplace_back(func(k, v));
      }
      return r;
    }

    void list_for_each(std::function<void(const Value&)>&& func) const;

    template<typename T>
    std::vector<T> list_map(std::function<T(Value&)>&& func) {
      std::vector<T> r;
      if (!isListValue()) return r;
      for(auto&  v : listvalue_) {
        r.emplace_back(func(v));
      }
      return r;
    }

    template<typename T>
    std::vector<T> list_map(std::function<T(const Value&)>&& func) const{
      std::vector<T> r;
      if (!isListValue()) return r;
      for(auto&  v : listvalue_) {
        r.push_back(func(v));
      }
      return r;
    }

    bool boolValue() const;

    int64_t intValue() const;
    
    double doubleValue() const;
    
    const std::string& stringValue() const;

    const std::map<std::string, Value>& objectValue() const;

    const std::vector<Value>& listValue() const;

    const std::vector<uint8_t>& byteArrayValue() const { 
      return bytevalue_;
    }

    const size_t byteArraySize() const {
      return bytevalue_.size();
    }

    Value& emplace(std::pair<std::string, Value>&& v) {
      typecode_ = VALUE_TYPE_OBJECT;
      objectvalue_.emplace(std::move(v));
      return *this;
    }

    Value& operator[](const std::string& key) {
      typecode_ = VALUE_TYPE_OBJECT;
      return objectvalue_[key];
    }

    Value operator[](const int key) {
      if (!isListValue()) {
        return Value::error("Value::operator[](" + std::to_string(key) + ") failed. Program tried to access as list access. But value is " + getTypeString() + " type.");
      }
      if (listvalue_.size() <= key) {
        return Value::error("Value::operator[](" + std::to_string(key) + ") failed. Program tried to access as list access. But list out of bounds.");
      } 
      return listvalue_[key];
    }


    const Value& at(const std::string& key) const ;

    Value& operator=(const Value& value) {
      typecode_ = value.typecode_;
      objectvalue_.clear();
      boolvalue_ = value.boolvalue_;
      intvalue_ = value.intvalue_;
      doublevalue_ = value.doublevalue_;
      objectvalue_ = value.objectvalue_;
      listvalue_ = value.listvalue_;
      stringvalue_ = value.stringvalue_;
      bytevalue_ = value.bytevalue_;
      return *this;
    }
  
    Value& operator=(Value&& value) {
      typecode_ = value.typecode_;
      objectvalue_.clear();
      boolvalue_ = value.boolvalue_;
      intvalue_ = value.intvalue_;
      doublevalue_ = value.doublevalue_;
      objectvalue_ = std::move(value.objectvalue_);
      listvalue_ = std::move(value.listvalue_);
      stringvalue_ = std::move(value.stringvalue_);
      bytevalue_ = std::move(value.bytevalue_);
      return *this;
    }
  
    Value& push_back(const Value& str) {
      typecode_ = VALUE_TYPE_LIST;
      objectvalue_.clear();
      listvalue_.push_back(str);
      return *this;
    }

    Value& emplace_back(Value&& val) {
      typecode_ = VALUE_TYPE_LIST;
      objectvalue_.clear();
      listvalue_.emplace_back(std::move(val));
      return *this;
    }
     
    bool operator==(const Value& v2) const {
      if (typecode_ != v2.typecode_) return false;
      if (isStringValue()) return stringValue() == v2.stringValue();
      if (isIntValue()) return intValue() == v2.intValue();
      if (isDoubleValue()) return doubleValue() == v2.doubleValue();
      if (isBoolValue()) return boolValue() == v2.boolValue();
      if (isListValue()) {
        if (listvalue_.size() != v2.listvalue_.size()) return false;
        for(size_t i = 0;i < listvalue_.size();i++) {
          if(listvalue_[i] != v2.listvalue_[i]) return false;
        }
        return true;
      }
      if (isObjectValue()) {
        if (objectvalue_.size() != v2.objectvalue_.size()) return false;
        for(const auto& [k, v] : objectvalue_) {
          if (v != v2.objectvalue_.at(k)) return false;
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
  };


  class value_pair : public std::pair<std::string, Value> {
  public:
    value_pair(const char* c, Value&& v): std::pair<std::string, Value>(c, std::move(v)) {}
    value_pair(const char* c, const int64_t v): value_pair(c, Value(v)) {}

  };


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
    static Value errorMessageValue;

#endif
    inline const Value& Value::at(const std::string& key) const {
      if (isError()) {
#ifdef ERROR_MESSAGE_LEVEL_FAST
        return errorError;
#else
        errorMessageValue = Value{ Value::VALUE_TYPE_ERROR, "Value::at() failed. Program tried to access with key value access. But value is ERROR type. (ErrorMessage is " + this->getErrorMessage() +")"};
        return errorMessageValue;
#endif
      }
      if (!isObjectValue()) {
#ifdef ERROR_MESSAGE_LEVEL_FAST
        return errorTypeError;
#else
        errorMessageValue = Value{ Value::VALUE_TYPE_ERROR, "Value::at() failed. Program tried to access with key value access. But value type is " + this->getTypeString() };
        return errorMessageValue;
#endif
      }
      if (objectvalue_.count(key) == 0) {
#ifdef ERROR_MESSAGE_LEVEL_FAST
        return errorKeyError;
#else
        errorMessageValue = Value{ Value::VALUE_TYPE_ERROR, "Value::at() failed. Program tried to access with key value access. But key (" + key + ") is not included."};
        return errorMessageValue;
#endif
      }
      return objectvalue_.at(key);
    }

} // namespace nerikiri


