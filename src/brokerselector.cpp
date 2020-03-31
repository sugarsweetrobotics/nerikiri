

#include "nerikiri/logger.h"
#include "nerikiri/brokerselector.h"


using namespace nerikiri;


BrokerSelector::BrokerSelector() {}

BrokerSelector::~BrokerSelector() {}


std::shared_ptr<BrokerAPI> BrokerSelector::create(const Value& value) {
  for(auto& b : brokerFactories_) {
    if (b->typeName() == value.at("name").stringValue()) {
      return b->create(value);
    }
  }
}