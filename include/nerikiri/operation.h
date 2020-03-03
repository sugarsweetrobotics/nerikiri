#pragma once

#include <map>
#include <functional>

#include "nerikiri/valuemap.h"
#include "operationinfo.h"

namespace nerikiri {

  class Operation {
  private:
    std::function<ValueMap(ValueMap)> function_;
    const OperationInfo info_;
  public:
    Operation(OperationInfo&& info, std::function<ValueMap(ValueMap)>&& func);
    Operation(const OperationInfo& info, std::function<ValueMap(ValueMap)>&& func);
    //Operation(std::map<std::string, std::string>&& info, std::function<ValueMap(ValueMap)>&& func): Operation(OperationInfo(std::forward<std::map<std::string,std::string>>(info)), std::forward<std::function<ValueMap(ValueMap)>>(func)) {}
    ~Operation();

  public:
    const OperationInfo& info() const { return info_; }

  };


}
