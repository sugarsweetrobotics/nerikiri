#pragma once

#include <string>
#include <condition_variable>

#include "nerikiri/broker.h"
#include "server.h"


namespace nerikiri::http {

  class HTTPBroker : public Broker {
  private:
    
  public:
    HTTPBroker(): Broker({"HTTPBroker"}) {};
    virtual ~HTTPBroker(){};
    
    virtual bool run() = 0;

    virtual void shutdown() = 0;
  };
  
  nerikiri::Broker_ptr broker(const std::string& address, const int32_t port); 
};
