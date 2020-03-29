#pragma once

#include "nerikiri/brokerapi.h"
#include "nerikiri/logger.h"

namespace nerikiri {


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

    virtual Value getConnectionInfos() const override {
      return requestResource("/process/connections/");
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