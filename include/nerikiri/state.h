#pragma once

#include <stdint.h>
#include <string>
#include <sstream>
#include <exception>
#include <map>
#include <optional>


namespace nerikiri {

  class ValueTypeError : public std::exception {
  private:
    std::string msg_;
    
  public:
    ValueTypeError(const char* msg) : msg_(msg) {}
    ValueTypeError(const std::string& msg) : msg_(msg) {}
    
    const char* what() const noexcept {
      std::stringstream ss;
      ss << "Invalid Value Data Access (" << msg_ << ")" << std::endl;
      return ss.str().c_str();
    }
  };


  /**
   *
   *
   */
  class Value {

  private:
    enum VALUE_TYPE_CODE {
			  VALUE_TYPE_NULL,
			  VALUE_TYPE_INT,
			  VALUE_TYPE_DOUBLE,
			  VALUE_TYPE_STRING,
			  VALUE_TYPE_OBJECT,
    };
    
  private:
    VALUE_TYPE_CODE typecode_;
    
    int64_t intvalue_;
    double doublevalue_;
    std::string stringvalue_;

    std::map<std::string, Value> objectvalue_;
  public:
    Value();
    Value(const int64_t value);
    Value(const double value);
    Value(const std::string& value);
    Value(const std::map<std::string, Value>& value);
    Value(const Value& Value);
    Value(Value&& Value);
    ~Value();
  private:
    
    void _clear() {
      typecode_ = VALUE_TYPE_NULL;
    }
    
  public:
    std::string getTypeString() const {
      if (isIntValue()) return "int";
      else if (isDoubleValue()) return "double";
      else if (isStringValue()) return "string";
      else if (isObjectValue()) return "object";
      return "null";
    }

    bool isIntValue() const { return typecode_ == VALUE_TYPE_INT; }
    
    bool isDoubleValue() const { return typecode_ == VALUE_TYPE_DOUBLE; }
    
    bool isStringValue() const { return typecode_ == VALUE_TYPE_STRING; }
    
    bool isObjectValue() const { return typecode_ == VALUE_TYPE_OBJECT; }
    
    bool hasKey(const std::string& key) const {
      if (!isObjectValue()) return false;
      return (objectvalue_.count(key) != 0);
    }
    
    int64_t intValue() const;
    
    double  doubleValue() const;
    
    const std::string& stringValue() const;

    Value& operator[](const std::string& key) {
      if (!isObjectValue()) throw new ValueTypeError(std::string("trying object value acecss. actual ") + getTypeString());
      return objectvalue_[key];
    }
        
    Value& operator=(const int64_t value) {
      typecode_ = VALUE_TYPE_INT;
      objectvalue_.clear();
      intvalue_ = value;
      return *this;
    }

    Value& operator=(const double value) {
      typecode_ = VALUE_TYPE_DOUBLE;
      objectvalue_.clear();
      doublevalue_ = value;
      return *this;
    }

    Value& operator=(const std::string& str) {
      typecode_ = VALUE_TYPE_STRING;
      objectvalue_.clear();
      stringvalue_ = str;
      return *this;
    }

  };


}
