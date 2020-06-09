#pragma once

#include <string>
#include <condition_variable>

#include "nerikiri/broker.h"
#include "nerikiri/brokerproxy.h"

#include "nerikiri/brokerfactory.h"

namespace nerikiri::http {

  class HTTPBroker : public Broker {
  public:
    HTTPBroker(): Broker({{"name", Value{"HTTPBroker"}}}) {};
    virtual ~HTTPBroker(){};
    
    virtual bool run(Process *process) = 0;

    virtual void shutdown(Process* process) = 0;
  };
  
  std::shared_ptr<nerikiri::Broker> broker(const std::string& address, const int32_t port); 

  class HTTPBrokerProxy : public AbstractBrokerProxy {
  public:
    HTTPBrokerProxy(const Value& info) : AbstractBrokerProxy(info) {}
    virtual ~HTTPBrokerProxy() {}
  };

  std::shared_ptr<nerikiri::Broker> brokerProxy(const std::string& address, const int32_t port); 

  class HTTPBrokerFactory : public nerikiri::BrokerFactory {
  public:

    HTTPBrokerFactory(): BrokerFactory({{"name", "HTTPBroker"}}) {}
    virtual ~HTTPBrokerFactory() {}

  public:
    virtual std::shared_ptr<Broker> create(const Value& param);
    
    virtual std::shared_ptr<BrokerAPI> createProxy(const Value& param);
  };
};
