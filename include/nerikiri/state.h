#pragma once

#include <stdint.h>
#include <string>
#include <sstream>
#include <exception>
#include <map>
#include <optional>
namespace nerikiri {

  class StateTypeError : public std::exception {
  private:
    std::string msg_;
    
  public:
    StateTypeError(const char* msg) : msg_(msg) {}
    StateTypeError(const std::string& msg) : msg_(msg) {}
    
    const char* what() const noexcept {
      std::stringstream ss;
      ss << "Invalid State Data Access (" << msg_ << ")" << std::endl;
      return ss.str().c_str();
    }
  };


  /**
   *
   *
   */
  class State {

  private:
    enum STATE_TYPE_CODE {
			  STATE_TYPE_NULL,
			  STATE_TYPE_INT,
			  STATE_TYPE_DOUBLE,
			  STATE_TYPE_STRING,
			  STATE_TYPE_OBJECT,
    };
    
  private:
    STATE_TYPE_CODE typecode_;
    
    int64_t intvalue_;
    double doublevalue_;
    std::string stringvalue_;

    std::map<std::string, State> objectvalue_;
  public:
    State();
    State(const int64_t value);
    State(const double value);
    State(const std::string& value);
    State(const std::map<std::string, State>& value);
    State(const State& state);
    State(State&& state);
    ~State();
  private:
    
    void _clear() {
      typecode_ = STATE_TYPE_NULL;
    }
    
  public:
    std::string getTypeString() const {
      if (isIntValue()) return "int";
      else if (isDoubleValue()) return "double";
      else if (isStringValue()) return "string";
      else if (isObjectValue()) return "object";
      return "null";
    }

    bool isIntValue() const { return typecode_ == STATE_TYPE_INT; }
    
    bool isDoubleValue() const { return typecode_ == STATE_TYPE_DOUBLE; }
    
    bool isStringValue() const { return typecode_ == STATE_TYPE_STRING; }
    
    bool isObjectValue() const { return typecode_ == STATE_TYPE_OBJECT; }
    
    bool hasKey(const std::string& key) const {
      if (!isObjectValue()) return false;
      return (objectvalue_.count(key) != 0);
    }
    
    int64_t intValue() const;
    
    double  doubleValue() const;
    
    const std::string& stringValue() const;

    State& operator[](const std::string& key) {
      if (!isObjectValue()) throw new StateTypeError(std::string("trying object value acecss. actual ") + getTypeString());
      return objectvalue_[key];
    }
        
    State& operator=(const int64_t value) {
      typecode_ = STATE_TYPE_INT;
      objectvalue_.clear();
      intvalue_ = value;
      return *this;
    }

    State& operator=(const double value) {
      typecode_ = STATE_TYPE_DOUBLE;
      objectvalue_.clear();
      doublevalue_ = value;
      return *this;
    }

    State& operator=(const std::string& str) {
      typecode_ = STATE_TYPE_STRING;
      objectvalue_.clear();
      stringvalue_ = str;
      return *this;
    }

  };


}
