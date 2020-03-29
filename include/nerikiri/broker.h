#pragma once

//#include "nerikiri/brokerinfo.h"
#include "nerikiri/logger.h"
#include "nerikiri/value.h"
#include "nerikiri/brokerapi.h"
#include <memory>


namespace nerikiri {

  class Process;
  using Process_ptr = Process*;

  class ProcessStore;
  
  class Connection;
  using ConnectionInfo = Value;

  using BrokerInfo = Value;


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
    ProcessStore* store_;

  public:
    static std::shared_ptr<BrokerAPI> null;
  public:
    Broker(const BrokerInfo& info): info_(info), process_(nullptr), store_(nullptr) {}
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
    
    virtual void setProcessStore(ProcessStore* store) override {
      logger::trace("Broker::setProcessStore()");
      store_ = store;
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

    virtual Value invokeOperationByName(const std::string& name) const override;

    virtual Value makeConnection(const ConnectionInfo& ci) const override;

    virtual Value registerConsumerConnection(const ConnectionInfo& ci) const override;

    virtual Value removeConsumerConnection(const ConnectionInfo& ci) const override;

    virtual Value pushViaConnection(const ConnectionInfo& ci, Value&& value)  const override;

    virtual Value requestResource(const std::string& path) const override;


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

    virtual void setProcessStore(ProcessStore* process) override {}
  };

  class AbstractBrokerProxy : public BrokerProxy {
  public:
    AbstractBrokerProxy() : BrokerProxy() {}
    virtual ~AbstractBrokerProxy() {}

  public:
    virtual BrokerInfo getBrokerInfo() const override {
      return requestResource("/broker/info/");
    }

    virtual Value getProcessInfo() const override {
      return requestResource("/process/info/");
    }

    virtual Value getOperationInfos() const override {
      return requestResource("/process/operations/");
    }

    virtual Value getContainerInfos() const override {
      return requestResource("/process/containers/");
    }

    virtual Value getContainerInfo(const Value& v) const override {
      return requestResource("/process/containers/" + v.at("name").stringValue() + "/");
    }

    virtual Value getContainerOperationInfos(const Value& v) const override {
      return requestResource("/process/containers/" + v.at("name").stringValue() + "/operations/");
    }

    virtual Value getContainerOperationInfo(const Value& ci, const Value& oi) const override {
      return requestResource("/process/containers/" + ci.at("name").stringValue() + "/operations/" + oi.at("name").stringValue() + "/");
    }

    virtual Value invokeContainerOperation(const Value& ci, const Value& oi) const override {
     return requestResource("/process/containers/" + ci.at("name").stringValue() + "/operations/" + oi.at("name").stringValue() + "/invoke/");
    }

    virtual Value getOperationInfo(const Value& v) const override {
      return requestResource("/process/operations/" + v.at("name").stringValue() + "/");
    }

    virtual Value invokeOperationByName(const std::string& name) const override {
      return requestResource("/process/operations/" + name + "/invoke/");
    }
  };

}
