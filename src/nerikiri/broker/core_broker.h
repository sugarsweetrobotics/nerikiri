#pragma once 

#include <string>
#include <memory>

#include <nerikiri/broker_api.h>

namespace nerikiri {
  class ProcessAPI;
  std::shared_ptr<BrokerFactoryAPI> coreBrokerFactory(ProcessAPI* process, const std::string& fullName);
}