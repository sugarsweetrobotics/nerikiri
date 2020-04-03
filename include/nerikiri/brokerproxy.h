#pragma once

#include "nerikiri/brokerapi.h"
#include "nerikiri/logger.h"

namespace nerikiri {


  class BrokerProxy : public BrokerAPI{
  public:
    BrokerProxy() {}
    virtual ~BrokerProxy() {}

  public:
    virtual bool run(Process* proc) override {return false;}
    
    virtual void shutdown(Process* proc) override {}

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

    virtual Value getConnectionInfos() const override {
      return requestResource("/process/connections/");
    }

    virtual Value getContainerInfo(const Value& v) const override {
      if (v.isError()) return v;    
      return requestResource("/process/containers/" + v.at("name").stringValue() + "/");
    }

    virtual Value getContainerOperationInfos(const Value& v) const override {
      if (v.isError()) return v;    
      return requestResource("/process/containers/" + v.at("name").stringValue() + "/operations/");
    }

    virtual Value getContainerOperationInfo(const Value& ci, const Value& oi) const override {
      if (ci.isError()) return ci;    
      return requestResource("/process/containers/" + ci.at("name").stringValue() + "/operations/" + oi.at("name").stringValue() + "/");
    }

    virtual Value invokeContainerOperation(const Value& ci, const Value& oi) const override {
      if (ci.isError()) return ci;    
      return requestResource("/process/containers/" + ci.at("name").stringValue() + "/operations/" + oi.at("name").stringValue() + "/invoke/");
    }

    virtual Value getOperationInfo(const Value& v) const override {
      if (v.isError()) return v;    
      return requestResource("/process/operations/" + v.at("name").stringValue() + "/");
    }

    virtual Value invokeOperationByName(const std::string& name) const override {
      return requestResource("/process/operations/" + name + "/invoke/");
    }

    virtual Value makeConnection(const ConnectionInfo& ci) override {
      return createResource("/process/connections/", ci);
    }


    virtual Value registerConsumerConnection(const ConnectionInfo& ci) override {
      if (ci.isError()) return ci;    
      auto operation_name = ci.at("input").at("info").at("name").stringValue();
      auto argument_name  = ci.at("input").at("target").at("name").stringValue();
      return createResource("/process/operations/" + operation_name + "/input/arguments/" + argument_name + "/connections/", ci);
    }

    virtual Value removeConsumerConnection(const ConnectionInfo& ci) override {
      if (ci.isError()) return ci;    
      auto operation_name = ci.at("input").at("info").at("name").stringValue();
      auto argument_name  = ci.at("input").at("target").at("name").stringValue();
      auto connection_name = ci.at("name").stringValue();
      return deleteResource("/process/operations/" + operation_name + "/input/arguments/" + argument_name + "/connections/" + connection_name + "/");
    }

    virtual Value registerProviderConnection(const ConnectionInfo& ci) override {
      if (ci.isError()) return ci;    
      auto operation_name = ci.at("input").at("info").at("name").stringValue();
      return createResource("/process/operations/" + operation_name + "/output/connections/", ci);
    }

    virtual Value removeProviderConnection(const ConnectionInfo& ci) override {
      auto operation_name = ci.at("input").at("info").at("name").stringValue();
      auto connection_name = ci.at("name").stringValue();
      return deleteResource("/process/operations/" + operation_name + "/output/connections/" + connection_name + "/");
    }
  };

}