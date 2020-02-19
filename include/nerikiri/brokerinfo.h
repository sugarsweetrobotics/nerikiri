#pragma once

#include <string>

namespace nerikiri {



  class BrokerInfo {
  public:
    std::string name;

  public:
    BrokerInfo(const std::string& name): name(name) {}
  };


  std::string str(const BrokerInfo& info);

};
