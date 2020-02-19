#pragma once

#include <string>
#include <condition_variable>

#include "nerikiri/broker.h"
#include "server.h"

namespace nerikiri::http {

  class HTTPBroker : public Broker {
  private:
    Server_ptr server_;
    std::condition_variable cond_;
    std::mutex mutex_;
  public:
    HTTPBroker(const std::string& address="0.0.0.0", const int32_t port=8080);
    virtual ~HTTPBroker();
    
    virtual bool run() override;

    virtual void shutdown() override;
  };

};
