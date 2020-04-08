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

  protected:
    Process_ptr process_;
    ProcessStore* store_;

  public:
    static std::shared_ptr<BrokerAPI> null;

  public:
    Broker(const BrokerInfo& info): BrokerAPI(info), process_(nullptr), store_(nullptr) {}
    virtual ~Broker() {}

    virtual bool run(Process* process) override{
      logger::trace("Broker::run()");
      return false;
    }
    
    virtual void shutdown(Process* process) override {
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

    virtual Value invokeOperation(const Value& v) const override;

    //virtual Value makeConnection(const ConnectionInfo& ci) override;

    virtual Value getConnectionInfos() const override;

    virtual Value registerConsumerConnection(const ConnectionInfo& ci) override;

    virtual Value registerProviderConnection(const Value& ci) override;
    
    virtual Value removeConsumerConnection(const ConnectionInfo& ci) override;

    virtual Value removeProviderConnection(const ConnectionInfo& ci) override;

    virtual Value putToArgument(const Value& opInfo, const std::string& argName, const Value& value) override;

    virtual Value putToArgumentViaConnection(const Value& conInfo, const Value& value) override;

    virtual Value requestResource(const std::string& path) const override;

    virtual Value createResource(const std::string& path, const Value& value) override;

    virtual Value deleteResource(const std::string& path) override;

  public:
    friend Value relayProvider(const Broker* broker, const ConnectionInfo& ci);

    friend Value checkDuplicateConsumerConnection(const Broker* broker, const ConnectionInfo& ci);
    
    friend Value registerConsumerConnection(const Broker_ptr broker, const ConnectionInfo& ci);
    
    friend Value removeConsumerConnection(const Broker_ptr broker, const ConnectionInfo& ci);

    friend Value nerikiri::registerProviderConnection(const Broker* broker, const ConnectionInfo& ci);
    
    friend Value makeConnection(BrokerAPI* broker, const ConnectionInfo& ci);
  };



}
