#pragma once

#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>
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
        VALUE_TYPE_LIST,
        VALUE_TYPE_ERROR,
    };
    
  private:
    VALUE_TYPE_CODE typecode_;
    
    int64_t intvalue_;
    double doublevalue_;
    std::string stringvalue_;

    std::map<std::string, Value> objectvalue_;
    std::vector<Value> listvalue_;
    std::string errormessage_;
  public:
    Value();

    //explicit Value(const int value);
    Value(const int64_t value);
    Value(int&& value);
    explicit Value(const double value);
    explicit Value(const std::string& value);
    Value(const char* value) : Value(std::string(value)) {}
    Value(std::string&& value);
    //explicit Value(const std::map<std::string, Value>& value);
    //Value(std::map<std::string, Value>&& value);
    Value(const Value& Value);
    //Value(Value&& Value) = default;
    Value(const std::vector<Value>& value);
    //Value(std::vector<Value>&& value);
    Value(std::vector<std::pair<std::string, Value>>&& value);
    Value(std::initializer_list<std::pair<std::string, Value>>&& vs);

    static Value error(const std::string& msg) {
      Value v;
      v.typecode_ = VALUE_TYPE_ERROR;
      v.errormessage_ = msg;
      return v;
    }
    virtual ~Value();
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
      else if (isListValue()) return "list";
      return "null";
    }

    bool isIntValue() const { return typecode_ == VALUE_TYPE_INT; }
    
    bool isDoubleValue() const { return typecode_ == VALUE_TYPE_DOUBLE; }
    
    bool isStringValue() const { return typecode_ == VALUE_TYPE_STRING; }
    
    bool isObjectValue() const { return typecode_ == VALUE_TYPE_OBJECT; }

    bool isListValue() const { return typecode_ == VALUE_TYPE_LIST; }
    
    bool isNull() const { return typecode_ == VALUE_TYPE_NULL; }

    bool isError() const { return typecode_ == VALUE_TYPE_ERROR; }

    std::string getErrorMessage() const { return errormessage_; }
    
    bool hasKey(const std::string& key) const {
      if (!isObjectValue()) return false;
      return (objectvalue_.count(key) != 0);
    }

    
    void object_for_each(std::function<void(const std::string&, Value&)>&& func) {
      for(auto& [k, v] : objectvalue_) {
        func(k, v);
      }
    }
    

    void object_for_each(std::function<void(const std::string&, const Value&)>&& func) const {
      for(const auto& [k, v] : objectvalue_) {
        func(k, v);
      }
    }

    template<typename T>
    std::vector<T> object_map(std::function<T(const std::string&, Value&)>&& func) {
      std::vector<T> r;
      for(auto& [k, v] : objectvalue_) {
        r.emplace_back(func(k, v));
      }
      return r;
    }

    template<typename T>
    std::vector<T> object_map(std::function<T(const std::string&, const Value&)>&& func) const {
      std::vector<T> r;
      for(const auto& [k, v] : objectvalue_) {
        r.emplace_back(func(k, v));
      }
      return r;
    }

    void list_for_each(std::function<void(Value&)>&& func) {
      for(auto& v: listvalue_) {
        func(v);
      }
    }

    template<typename T>
    std::vector<T> list_map(std::function<T(Value&)>&& func) {
      std::vector<T> r;
      for(auto&  v : listvalue_) {
        r.emplace_back(func(v));
      }
      return r;
    }

    int64_t intValue() const;
    
    double  doubleValue() const;
    
    const std::string& stringValue() const;

    const std::map<std::string, Value>& objectValue() const;

    const std::vector<Value>& listValue() const;

    Value& emplace(std::pair<std::string, Value>&& v) {
      typecode_ = VALUE_TYPE_OBJECT;
      //if (!isObjectValue()) throw new ValueTypeError(std::string("trying object value acecss. actual ") + getTypeString());
      objectvalue_.emplace(std::move(v));
      return *this;
    }

    Value& operator[](const std::string& key) {
      typecode_ = VALUE_TYPE_OBJECT;
      //if (!isObjectValue()) throw new ValueTypeError(std::string("trying object value acecss. actual ") + getTypeString());
      return objectvalue_[key];
    }

    Value& operator[](const int key) {
      if (!isListValue()) throw new ValueTypeError(std::string("trying list value acecss. actual ") + getTypeString());
      return listvalue_[key];
    }

    const Value& at(const std::string& key) const {
      if (!isObjectValue()) throw new ValueTypeError(std::string("trying object value acecss. actual ") + getTypeString());
      return objectvalue_.at(key);
    }
    /*
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
     */

    Value& operator=(const Value& value) {
      typecode_ = value.typecode_;
      objectvalue_.clear();
      intvalue_ = value.intvalue_;
      doublevalue_ = value.doublevalue_;
      objectvalue_ = value.objectvalue_;
      listvalue_ = value.listvalue_;
      stringvalue_ = value.stringvalue_;
      return *this;
    }
  
    Value& operator=(Value&& value) {
      typecode_ = value.typecode_;
      objectvalue_.clear();
      intvalue_ = value.intvalue_;
      doublevalue_ = value.doublevalue_;
      objectvalue_ = value.objectvalue_;
      listvalue_ = value.listvalue_;
      stringvalue_ = value.stringvalue_;
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
  };

  class value_pair : public std::pair<std::string, Value> {
  public:
    value_pair(const char* c, Value&& v): std::pair<std::string, Value>(c, std::move(v)) {}
    value_pair(const char* c, const int64_t v): value_pair(c, Value(v)) {}

  };

  inline Value errorValue(const std::string& msg) {
   return Value::error(msg);
  }


  std::string str(const Value& value);

}


