#pragma once

#include "nerikiri/brokerinfo.h"
#include "nerikiri/logger.h"
#include "nerikiri/value.h"
#include <memory>

namespace nerikiri {

  class Process;
  using Process_ptr = Process*;
  
  class Broker {
  private:
    const BrokerInfo info_;
  public:
    const BrokerInfo& info() const { return info_; }

  protected:
    Process_ptr process_;
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

    virtual void setProcess(Process_ptr process) {
      logger::trace("Broker::setProcess()");
      process_ = process;
    }
    
  };
  using Broker_ptr = std::unique_ptr<Broker>;

}
