#include "nerikiri/logger.h"
#include "nerikiri/brokerdictionary.h"


using namespace nerikiri;


void BrokerDictionary::add(Broker_ptr&& broker) {
  logger::trace("BrokerDictionary::add({})", broker->info().name);
  dictionary_.emplace(broker->info().name, std::move(broker));
  logger::trace("BrokerDictionary::add ends");
}

void BrokerDictionary::foreach(std::function<void(Broker_ptr&)> func) {
  logger::trace("BrokerDictionary::foreach()");
  for(auto& [name, broker] : dictionary_) {
    logger::trace(" - for broker {}", str(broker->info()));
    func(broker);
  }
  logger::trace("BrokerDictionary::foreach ends");
}
    

