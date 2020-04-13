#pragma once

#include "nerikiri/brokers/brokerapi.h"

namespace nerikiri {


  class AbstractBrokerProxy : public BrokerAPI {
  public:
    AbstractBrokerProxy(const Value& v) : BrokerAPI(v) {}
    virtual ~AbstractBrokerProxy() {}

  public:
    virtual Value getBrokerInfo() const override {
      return readResource("/broker/info/");
    }

    virtual Value getProcessInfo() const override {
      return readResource("/process/info/");
    }

    virtual Value getOperationInfos() const override {
      return readResource("/process/operations/");
    }

    virtual Value getContainerInfos() const override {
      return readResource("/process/containers/");
    }

    virtual Value getConnectionInfos() const override {
      return readResource("/process/connections/");
    }

    virtual Value getContainerInfo(const Value& v) const override {
      if (v.isError()) return v;    
      return readResource("/process/containers/" + v.at("instanceName").stringValue() + "/");
    }

    virtual Value getContainerOperationInfos(const Value& v) const override {
      if (v.isError()) return v;    
      return readResource("/process/containers/" + v.at("instanceName").stringValue() + "/operations/");
    }

    virtual Value getContainerOperationInfo(const Value& ci, const Value& oi) const override {
      if (ci.isError()) return ci;    
      return readResource("/process/containers/" + ci.at("instanceName").stringValue() + "/operations/" + oi.at("instanceName").stringValue() + "/info/");
    }

    virtual Value invokeContainerOperation(const Value& ci, const Value& oi) const override {
      if (ci.isError()) return ci;    
      return readResource("/process/containers/" + ci.at("instanceName").stringValue() + "/operations/" + oi.at("instanceName").stringValue() + "/");
    }

    virtual Value executeOperation(const Value& info) override {
      if (info.isError()) return info;    
      return updateResource("/process/operations/" + info.at("instanceName").stringValue() + "/execution/", {});
    }

    virtual Value getOperationInfo(const Value& v) const override {
      if (v.isError()) return v;    
      return readResource("/process/operations/" + v.at("instanceName").stringValue() + "/info/");
    }

    virtual Value invokeOperation(const Value& v) const override {
      return readResource("/process/operations/" + v.at("instanceName").stringValue() + "/");
    }

    virtual Value registerConsumerConnection(const ConnectionInfo& ci) override {
      if (ci.isError()) return ci;    
      auto operation_name = ci.at("input").at("info").at("instanceName").stringValue();
      auto argument_name  = ci.at("input").at("target").at("name").stringValue();
      return createResource("/process/operations/" + operation_name + "/input/arguments/" + argument_name + "/connections/", ci);
    }

    virtual Value removeConsumerConnection(const ConnectionInfo& ci) override {
      if (ci.isError()) return ci;    
      auto operation_name = ci.at("input").at("info").at("instanceName").stringValue();
      auto argument_name  = ci.at("input").at("target").at("name").stringValue();
      auto connection_name = ci.at("name").stringValue();
      return deleteResource("/process/operations/" + operation_name + "/input/arguments/" + argument_name + "/connections/" + connection_name + "/");
    }

    virtual Value registerProviderConnection(const ConnectionInfo& ci) override {
      if (ci.isError()) {
        logger::error("BrokerProxy::registerProviderConnection failed. ({})", str(ci));
        return ci;
      }
      if (ci.at("output").at("info").isError()) {
        logger::error("BrokerProxy::registerProviderConnection failed. ({})", str(ci));
        return ci.at("output").at("info");
      }
      if (ci.at("input").at("info").isError()) {
        logger::error("BrokerProxy::registerProviderConnection failed. ({})", str(ci));
        return ci.at("input").at("info");
      }
      auto operation_name = ci.at("input").at("info").at("instanceName").stringValue();
      return createResource("/process/operations/" + operation_name + "/output/connections/", ci);
    }

    virtual Value removeProviderConnection(const ConnectionInfo& ci) override {
      auto operation_name = ci.at("input").at("info").at("instanceName").stringValue();
      auto connection_name = ci.at("name").stringValue();
      return deleteResource("/process/operations/" + operation_name + "/output/connections/" + connection_name + "/");
    }

    virtual Value putToArgumentViaConnection(const Value& conInfo, const Value& value) override {
      if (conInfo.isNull()) {
        return Value::error(logger::error("HTTPBrokerProxyImpl::putToArgumentViaConnection failed. Connection is null."));
      }
      const auto operation_name = conInfo.at("input").at("info").at("instanceName").stringValue();
      const auto connection_name = conInfo.at("name").stringValue();
      const auto argument_name = conInfo.at("input").at("target").at("name").stringValue();
      return updateResource("/process/operations/" + operation_name + "/input/arguments/" + argument_name + "/connections/" + connection_name + "/", value);
    }
  };

}