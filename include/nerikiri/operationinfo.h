#pragma once

#include <string>
#include <map>
#include <vector>
#include "nerikiri/value.h"

namespace nerikiri {
  /*
  class OperationInfo : public Value {
  public:

  public:
    // OperationInfo(Value&& v): Value(std::move(v)) {}


    //OperationInfo(std::map<std::string, Value>&& info): Value(std::move(info)) {}
    
    *
    OperationInfo(std::map<std::string,std::string>&& info): Value() {
      for(auto [ik, iv] : info) {
        this->emplace({ik, Value(iv)});
      }
    }*

public:
    const std::string name() const { return this->at("name").stringValue(); }
  };
  */

  using OperationInfo = Value;
  
  class OperationInfos : public Value {
  public:
    OperationInfos() : Value(std::vector<Value>()) {}
    OperationInfos(std::vector<Value>&& infos) : Value(std::move(infos)){}
  };

  
  std::string str(const OperationInfo& info);
};
