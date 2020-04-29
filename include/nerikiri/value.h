/**
 * nerikiriで使う基本的なデータ型
 */

#pragma once

#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>
#include <exception>
#include <map>
#include <optional>
#include <functional>
#include <algorithm>
#include "nerikiri/nerikiri.h"

namespace nerikiri {

  class ValueTypeError : public std::exception {
  private:
    std::string msg_;
    
  public:
    ValueTypeError(const char* msg) : msg_(msg) {}
    ValueTypeError(const std::string& msg) : msg_(msg) {}
    
    const char* what() const noexcept {
      return ("Invalid Value Data Access (" + msg_ + ")").c_str();
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

  private:
    enum VALUE_TYPE_CODE {
			  VALUE_TYPE_NULL,
			  VALUE_TYPE_INT,
        VALUE_TYPE_BOOL,
			  VALUE_TYPE_DOUBLE,
			  VALUE_TYPE_STRING,
			  VALUE_TYPE_OBJECT,
        VALUE_TYPE_LIST,
        VALUE_TYPE_ERROR,
    };
    
  private:
    VALUE_TYPE_CODE typecode_;
    
    bool boolvalue_;
    int64_t intvalue_;
    double doublevalue_;
    std::string stringvalue_;
    std::map<std::string, Value> objectvalue_;
    std::vector<Value> listvalue_;
    
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


    explicit Value(const std::vector<float>& dbls);

    explicit Value(const std::vector<double>& dbls);

    explicit Value(const std::vector<bool>& bls);

    static Value error(const std::string& msg) {
      Value v;
      v.typecode_ = VALUE_TYPE_ERROR;
      v.errormessage_ = msg;
      return v;
    }
    
    virtual ~Value();

    static Value list() {
      Value v;
      v.typecode_ = VALUE_TYPE_LIST;
      return v;
    }

    static Value object() {
      Value v;
      v.typecode_ = VALUE_TYPE_OBJECT;
      return v;
    }
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
      else if (isBoolValue()) return "bool";
      return "null";
    }

    bool isBoolValue() const { return typecode_ == VALUE_TYPE_BOOL; }

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

    void list_for_each(std::function<void(const Value&)>&& func) const {
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

    bool boolValue() const;

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
      if (!isListValue()) throw ValueTypeError(std::string("trying list value acecss. actual ") + getTypeString());
      if (listvalue_.size() <= key) throw ValueTypeError(std::string("Value list access failed. Index is exceeded from size"));
      return listvalue_[key];
    }

    const Value& at(const std::string& key) const {
      if (!isObjectValue()) throw ValueTypeError(std::string("trying object value acecss. actual ") + getTypeString());
      if (objectvalue_.count(key) == 0) {
        throw ValueTypeError(std::string("trying object access in key(" + key + ") but not found. Value is " + str(*this)));
      }
      return objectvalue_.at(key);
    }

    Value& operator=(const Value& value) {
      typecode_ = value.typecode_;
      objectvalue_.clear();
      boolvalue_ = value.boolvalue_;
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
      boolvalue_ = value.boolvalue_;
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
  };

  class value_pair : public std::pair<std::string, Value> {
  public:
    value_pair(const char* c, Value&& v): std::pair<std::string, Value>(c, std::move(v)) {}
    value_pair(const char* c, const int64_t v): value_pair(c, Value(v)) {}

  };

  inline Value errorValue(const std::string& msg) {
   return Value::error(msg);
  }

  inline Value merge(const Value& v1, const Value& v2) {
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
//      result.insert(result.end(), v1.listvalue_.begin(), v1.listvalue_.end());
      //      std::merge(v1.listvalue_.begin(), v1.listvalue_.end(),
      //       v2.listvalue_.begin(), v2.listvalue_.end(), std::back_inserter(result));
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
//    return Value::error("Value::merge failed. Invalid Value types of arguments v1 and v2.");
  }

  inline std::string str(const nerikiri::Value& value) {
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
    if (value.isNull()) {
        return "{}";
    }
    if (value.isError()) {
    return "{\"Error\": \"Value is error('" + value.getErrorMessage() + "').\"}";
        //throw ValueTypeError(value.getErrorMessage());
    }
    return "{\"Error\": \"Value is not supported type.\"}";
  }

//inline Value::Value(bool&& value) : typecode_(VALUE_TYPE_BOOL), boolvalue_(std::move(value)) {}
inline Value::Value(int&& value) : typecode_(VALUE_TYPE_INT), intvalue_(std::move(value)) {}
inline Value::Value(const bool value) : typecode_(VALUE_TYPE_BOOL), boolvalue_(value) {}
inline Value::Value(const int64_t value) : typecode_(VALUE_TYPE_INT), intvalue_(value) {}
inline Value::Value(const double value) : typecode_(VALUE_TYPE_DOUBLE), doublevalue_(value) {}
inline Value::Value(const std::string& value) : typecode_(VALUE_TYPE_STRING), stringvalue_(value) {}
inline Value::Value(std::string&& value) : typecode_(VALUE_TYPE_STRING), stringvalue_(std::move(value)) {}
inline Value::Value(const std::vector<Value>& value) : typecode_(VALUE_TYPE_LIST), listvalue_(value) {}
inline Value::Value(const Value& value): typecode_(value.typecode_), boolvalue_(value.boolvalue_), intvalue_(value.intvalue_), doublevalue_(value.doublevalue_), stringvalue_(value.stringvalue_), objectvalue_(value.objectvalue_), listvalue_(value.listvalue_) {}

inline Value::Value(const std::vector<float>& dbls) : typecode_(VALUE_TYPE_LIST) {
  for(auto v : dbls) {
    listvalue_.emplace_back(Value((double)v));
  }
}

inline Value::Value(const std::vector<double>& dbls) : typecode_(VALUE_TYPE_LIST) {
  for(auto v : dbls) {
    listvalue_.emplace_back(Value(v));
  }
}

inline Value::Value(const std::vector<bool>& bls) : typecode_(VALUE_TYPE_LIST) {
  for(auto v : bls) {
    Value val{v};
    listvalue_.emplace_back(val);
  }
}

inline Value::Value(std::vector<std::pair<std::string, Value>>&& ps): typecode_(VALUE_TYPE_OBJECT) {
  for(auto &p : ps) {
    objectvalue_[p.first] = p.second;
  }
}

inline Value::Value(std::initializer_list<std::pair<std::string, Value>>&& ps) : typecode_(VALUE_TYPE_OBJECT) {
  for(auto &p : ps) {
    objectvalue_[p.first] = p.second;
  }
}

inline Value::Value(): typecode_(VALUE_TYPE_NULL){}

inline Value::~Value() {}

inline bool Value::boolValue() const {
  if (isBoolValue()) return boolvalue_;
  throw new ValueTypeError(std::string("trying bool value acecss. actual ") + getTypeString());
}

inline int64_t Value::intValue() const {
  if (isIntValue()) return intvalue_;
  throw new ValueTypeError(std::string("trying int value acecss. actual ") + getTypeString());
}

inline double Value::doubleValue() const {
  if (isDoubleValue()) return doublevalue_;
  throw new ValueTypeError(std::string("trying double value acecss. actual ") + getTypeString());
}

inline const std::string& Value::stringValue() const {
  if (isStringValue()) return stringvalue_;
  throw new ValueTypeError(std::string("trying string value acecss. actual ") + getTypeString());
}

inline const std::map<std::string, Value>& Value::objectValue() const {
  if (isObjectValue()) return objectvalue_;
  throw new ValueTypeError(std::string("trying object value acecss. actual ") + getTypeString());
}

inline const std::vector<Value>& Value::listValue() const {
  if (isListValue()) return listvalue_;
  throw new ValueTypeError(std::string("trying list value acecss. actual ") + getTypeString());
}

}


