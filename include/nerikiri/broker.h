#pragma once

//#include "nerikiri/brokerinfo.h"
#include "nerikiri/logger.h"
#include "nerikiri/value.h"
#include <memory>

namespace nerikiri {

  class Process;
  using Process_ptr = Process*;
  

  using BrokerInfo = Value;

  class BrokerAPI {
  private:

  public:
    BrokerAPI() {}
    virtual ~BrokerAPI() {}

    virtual bool run() = 0;
    
    virtual void shutdown() = 0;

    virtual void setProcess(Process_ptr process)  = 0;

    virtual BrokerInfo info() const = 0; 

    virtual Value getProcessInfo() const = 0;

    virtual Value getProcessOperationInfos() const = 0;

    virtual Value getOperationInfoByName(const std::string& name) const = 0;

    virtual Value callOperationByName(const std::string& name, Value&& value) const = 0;

    virtual Value invokeOperationByName(const std::string& name) const = 0;
  };

  class Broker  : public BrokerAPI{
  private:
    const BrokerInfo info_;

  protected:
    Process_ptr process_;

  public:
    static std::unique_ptr<BrokerAPI> null;
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
    
  public:
    virtual BrokerInfo info() const { return info_; }

    virtual Value getProcessInfo() const;

    virtual Value getProcessOperationInfos() const;

    virtual Value getOperationInfoByName(const std::string& name) const;

    virtual Value callOperationByName(const std::string& name, Value&& value) const;

    virtual Value invokeOperationByName(const std::string& name) const;
  };

  using Broker_ptr = std::unique_ptr<BrokerAPI>;

  class BrokerProxy : public BrokerAPI{
  public:
    BrokerProxy() {}
    virtual ~BrokerProxy() {}

  public:
    virtual bool run() {return false;}
    
    virtual void shutdown() {}

    virtual void setProcess(Process_ptr process) {}
    
  };

}
