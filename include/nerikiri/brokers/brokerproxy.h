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

    virtual Value registerConsumerConnection(const Value& ci) override {
      if (ci.isError()) return ci;    
      auto operation_name = ci.at("input").at("info").at("instanceName").stringValue();
      auto argument_name  = ci.at("input").at("target").at("name").stringValue();
      return createResource("/process/operations/" + operation_name + "/input/arguments/" + argument_name + "/connections/", ci);
    }

    virtual Value removeConsumerConnection(const Value& ci) override {
      if (ci.isError()) return ci;    
      auto operation_name = ci.at("input").at("info").at("instanceName").stringValue();
      auto argument_name  = ci.at("input").at("target").at("name").stringValue();
      auto connection_name = ci.at("name").stringValue();
      return deleteResource("/process/operations/" + operation_name + "/input/arguments/" + argument_name + "/connections/" + connection_name + "/");
    }

    virtual Value registerProviderConnection(const Value& ci) override {
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

    virtual Value removeProviderConnection(const Value& ci) override {
      auto operation_name = ci.at("input").at("info").at("instanceName").stringValue();
      auto connection_name = ci.at("name").stringValue();
      return deleteResource("/process/operations/" + operation_name + "/output/connections/" + connection_name + "/");
    }

    virtual Value callContainerOperation(const Value& ci, const Value& oi, Value&& arg) override {
     return updateResource("/process/container/" + ci.at("name").stringValue() + "/operation/" + oi.at("name").stringValue() + "/", arg);
    }

    virtual Value callOperation(const Value& info, Value&& value) override {
      return updateResource("/process/operation/" + info.at("name").stringValue() + "/", value);
    }

    virtual Value putToArgument(const Value& opInfo, const std::string& argName, const Value& value) override {
      auto operation_name = opInfo.at("instanceName").stringValue();
      return updateResource("/process/operations/" + operation_name + "/input/arguments/" + argName + "/", value);
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

    virtual Value getOperationFactoryInfos() const override {
      return readResource("/process/operationFactories/");
      
    }

    virtual Value getContainerFactoryInfos() const override {
      return readResource("/process/containerFactories/");
    }

    virtual Value createOperation(const Value& value) override {
      return createResource("/process/operations/", value);
    }

    virtual Value createContainer(const Value& value) override {
      return createResource("/process/containers/", value);
    }

  };


  class NullBrokerProxy : public AbstractBrokerProxy {
  public:

    NullBrokerProxy() : AbstractBrokerProxy({{"name", "NullBrokerProxy"}}) {}
    virtual ~NullBrokerProxy() {}

  public:
    virtual Value readResource(const std::string& path) const override {
      return Value::error("BrokerProxy::readResource failed. BrokerProxy is NullBrokerProxy.");
    }

    virtual Value createResource(const std::string& path, const Value& value) override {
      return Value::error("BrokerProxy::createResource failed. BrokerProxy is NullBrokerProxy.");
    }

    virtual Value updateResource(const std::string& path, const Value& value) override {
      return Value::error("BrokerProxy::updateResource failed. BrokerProxy is NullBrokerProxy.");
    }

    virtual Value deleteResource(const std::string& path) override {
      return Value::error("BrokerProxy::deleteResource failed. BrokerProxy is NullBrokerProxy.");
    }

  };
}