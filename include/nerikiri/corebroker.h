#pragma once 

#include <string>
#include <memory>

#include <nerikiri/broker_factory_api.h>

namespace nerikiri {
  class Process;
  std::shared_ptr<BrokerFactoryAPI> coreBrokerFactory(Process* process, const std::string& fullName);
}