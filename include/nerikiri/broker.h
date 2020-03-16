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

    virtual Value getBrokerInfo() const = 0; 

    virtual Value getProcessInfo() const = 0;

    virtual Value getOperationInfos() const = 0;

    virtual Value getOperationInfo(const Value& name) const = 0;

    virtual Value getContainerInfos() const = 0;
    
    virtual Value getContainerInfo(const Value& value) const = 0;

    virtual Value getContainerOperationInfos(const Value& value) const = 0;

    virtual Value getContainerOperationInfo(const Value& cinfo, const Value& oinfo) const  = 0;

    virtual Value callContainerOperation(const Value& cinfo, const Value& oinfo, Value&& arg) const  = 0;

    virtual Value invokeContainerOperation(const Value& cinfo, const Value& oinfo) const  = 0;

    virtual Value callOperation(const Value& info, Value&& value) const = 0;

    virtual Value invokeOperationByName(const std::string& name) const = 0;

    virtual Value makeConnection(const ConnectionInfo& ci) const = 0;

    virtual Value registerConsumerConnection(const ConnectionInfo& ci) const = 0;

    virtual Value removeConsumerConnection(const ConnectionInfo& ci) const = 0;

    virtual Value pushToArgumentByName(const std::string& operation_name, const std::string& argument_name, Value&& value)  const = 0;
  };

  using Broker_ptr = std::shared_ptr<BrokerAPI>;

  class Broker;

  Value relayProvider(const Broker* broker, const ConnectionInfo& ci);
  Value checkDuplicateConsumerConnection(const Broker* broker, const ConnectionInfo& ci);

  Value registerConsumerConnection(const Broker_ptr broker, const ConnectionInfo& ci);
  Value removeConsumerConnection(const Broker_ptr broker, const ConnectionInfo& ci);
  Value registerProviderConnection(const Broker* broker, const ConnectionInfo& ci);
   
  Value makeConnection(const BrokerAPI* broker, const ConnectionInfo& ci);

  class Broker  : public BrokerAPI{
  private:
    const BrokerInfo info_;

  protected:
    Process_ptr process_;

  public:
    static std::shared_ptr<BrokerAPI> null;
  public:
    Broker(const BrokerInfo& info): info_(info), process_(nullptr) {}
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
    virtual BrokerInfo getBrokerInfo() const override { return info_; }

    virtual Value getProcessInfo() const override;

    virtual Value getOperationInfos() const override;

    virtual Value getContainerInfos() const override;

    virtual Value getContainerInfo(const Value& info) const override;

    virtual Value getContainerOperationInfos(const Value& info) const override;

    virtual Value getContainerOperationInfo(const Value& cinfo, const Value& oinfo) const override;

    virtual Value callContainerOperation(const Value& cinfo, const Value& oinfo, Value&& arg) const override;

    virtual Value invokeContainerOperation(const Value& cinfo, const Value& oinfo) const override;

    virtual Value getOperationInfo(const Value& info) const override;


    virtual Value callOperation(const Value& info, Value&& value) const override;

    //virtual Value callOperationByName(const std::string& name, Value&& value) const override;

    virtual Value invokeOperationByName(const std::string& name) const override;

    virtual Value makeConnection(const ConnectionInfo& ci) const override;

    virtual Value registerConsumerConnection(const ConnectionInfo& ci) const override;

    virtual Value removeConsumerConnection(const ConnectionInfo& ci) const override;

    virtual Value pushToArgumentByName(const std::string& operation_name, const std::string& argument_name, Value&& value)  const override;


  public:
    friend Value relayProvider(const Broker* broker, const ConnectionInfo& ci);

    friend Value checkDuplicateConsumerConnection(const Broker* broker, const ConnectionInfo& ci);
    
    friend Value registerConsumerConnection(const Broker_ptr broker, const ConnectionInfo& ci);
    
    friend Value removeConsumerConnection(const Broker_ptr broker, const ConnectionInfo& ci);

    friend Value nerikiri::registerProviderConnection(const Broker* broker, const ConnectionInfo& ci);
    
    friend Value makeConnection(const BrokerAPI* broker, const ConnectionInfo& ci);
  };


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
