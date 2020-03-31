#pragma once

#include <string>
#include <condition_variable>

#include "nerikiri/broker.h"
#include "nerikiri/brokerproxy.h"
#include "nerikiri/http/server.h"


namespace nerikiri::http {

  class HTTPBroker : public Broker {
  public:
    HTTPBroker(): Broker({{"name", Value{"HTTPBroker"}}}) {};
    virtual ~HTTPBroker(){};
    
    virtual bool run() = 0;

    virtual void shutdown() = 0;
  };
  
  nerikiri::Broker_ptr broker(const std::string& address, const int32_t port); 

  class HTTPBrokerProxy : public AbstractBrokerProxy {
  public:
    HTTPBrokerProxy() {}
    virtual ~HTTPBrokerProxy() {}
  };

  nerikiri::Broker_ptr brokerProxy(const std::string& address, const int32_t port); 

  class HTTPBrokerFactory : public nerikiri::BrokerFactory {
  public:

    HTTPBrokerFactory(): BrokerFactory({{"name", "HTTPBroker"}}) {}
    virtual ~HTTPBrokerFactory() {}

  public:
    virtual std::shared_ptr<BrokerAPI> create(const Value& param);
  };
};
