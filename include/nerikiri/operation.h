#pragma once

#include <map>
#include <functional>

#include "nerikiri/statemap.h"
#include "operationinfo.h"

namespace nerikiri {

  class Operation {
  private:
    std::function<StateMap(StateMap)> function_;
    const OperationInfo info_;
  public:
    Operation(OperationInfo&& info, std::function<StateMap(StateMap)>&& func);
    Operation(std::map<std::string, std::string>&& info, std::function<StateMap(StateMap)>&& func): Operation(OperationInfo(std::forward<std::map<std::string,std::string>>(info)), std::forward<std::function<StateMap(StateMap)>>(func)) {}
    ~Operation();

  public:
    

  };


}
