#pragma once

#include <memory>
#include "nerikiri/value.h"
#include "nerikiri/broker.h"

namespace nerikiri {

  class BrokerAPI;
  class Broker;

  class BrokerFactory {
  private:
    const std::string typeName_;
  public:
    BrokerFactory() : typeName_("NullBroker") {}
    BrokerFactory(const Value& value) : typeName_(value.at("name").stringValue()) {}

    virtual ~BrokerFactory() {}

  public:
    const std::string& typeName() { return typeName_; }
    
  public:
    virtual std::shared_ptr<Broker> create(const Value& param) {
        return Broker::null;
    }

    virtual std::shared_ptr<BrokerAPI> createProxy(const Value& param) {
        return std::shared_ptr<BrokerAPI>(new NullBroker());
    }

    static std::shared_ptr<BrokerFactory> null;
  };

}