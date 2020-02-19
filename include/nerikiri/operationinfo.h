#pragma once

#include <string>
#include <map>

namespace nerikiri {

  class OperationInfo {
  public:
    const std::string name;


  public:
    OperationInfo(const std::string& name): name(name) {}
    OperationInfo(std::map<std::string,std::string>&& info): name(info["name"]) {

    }
  };

  std::string str(const OperationInfo& info);
};
