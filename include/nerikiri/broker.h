#pragma once

#include "brokerinfo.h"
#include "logger.h"
#include <memory>

namespace nerikiri {

  class Broker {
  private:
    const BrokerInfo info_;
  public:
    const BrokerInfo& info() const { return info_; }
    
  public:
    Broker(const BrokerInfo& info): info_(info) {}
    virtual ~Broker() {}

    virtual bool run() {
      logger::trace("Broker::run()");
      return false;
    }
    
    virtual void shutdown() {
      logger::trace("Broker::shutdown()");
    }
    
  };
  using Broker_ptr = std::unique_ptr<Broker>;

}
