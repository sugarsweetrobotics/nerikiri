#pragma once

//#include "nerikiri/brokerinfo.h"
#include "nerikiri/logger.h"
#include "nerikiri/value.h"
//#include "nerikiri/connection.h"
#include <memory>

namespace nerikiri {

  class Process;
  using Process_ptr = Process*;
  
  class Connection;
  using ConnectionInfo = Value;

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

    virtual Value makeConnection(const ConnectionInfo& ci) const = 0;

    virtual Value registerConsumerConnection(const ConnectionInfo& ci) const = 0;

    virtual Value removeConsumerConnection(const ConnectionInfo& ci) const = 0;
  };

  class Broker  : public BrokerAPI{
  private:
    const BrokerInfo info_;

  protected:
    Process_ptr process_;

  public:
    static std::shared_ptr<BrokerAPI> null;
  public:
    Broker(const BrokerInfo& info): info_(info) {}
    virtual ~Broker() {}

    virtual bool run() override{
      logger::trace("Broker::run()");
      return false;
    }
    
    virtual void shutdown() override {
      logger::trace("Broker::shutdown()");
    }

    virtual void setProcess(Process_ptr process) override {
      logger::trace("Broker::setProcess()");
      process_ = process;
    }
    
  public:
    virtual BrokerInfo info() const override { return info_; }

    virtual Value getProcessInfo() const override;

    virtual Value getProcessOperationInfos() const override;

    virtual Value getOperationInfoByName(const std::string& name) const override;

    virtual Value callOperationByName(const std::string& name, Value&& value) const override;

    virtual Value invokeOperationByName(const std::string& name) const override;

    virtual Value makeConnection(const ConnectionInfo& ci) const override;

    virtual Value registerConsumerConnection(const ConnectionInfo& ci) const override;

    virtual Value removeConsumerConnection(const ConnectionInfo& ci) const override;
  };

  using Broker_ptr = std::shared_ptr<BrokerAPI>;

  class BrokerProxy : public BrokerAPI{
  public:
    BrokerProxy() {}
    virtual ~BrokerProxy() {}

  public:
    virtual bool run() override {return false;}
    
    virtual void shutdown() override {}

    virtual void setProcess(Process_ptr process) override {}
    
  };

}
