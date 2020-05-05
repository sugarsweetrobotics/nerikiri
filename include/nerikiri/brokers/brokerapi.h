#pragma once

#include <string>

#include "nerikiri/value.h"
#include "nerikiri/object.h"

namespace nerikiri {

  class BrokerAPI : public Object {
  private:

  public:
    BrokerAPI(): Object() {}

    BrokerAPI(const Value& info) : Object(info) {}

    virtual ~BrokerAPI() {}

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

    virtual Value invokeOperation(const Value& name) const = 0;

    virtual Value executeOperation(const Value& info) = 0;

    virtual Value getConnectionInfos() const = 0;

    virtual Value registerConsumerConnection(const Value& ci)  = 0;

    virtual Value registerProviderConnection(const Value& ci)  = 0;

    virtual Value removeProviderConnection(const Value& ci) = 0;

    virtual Value removeConsumerConnection(const Value& ci) = 0;

    virtual Value putToArgument(const Value& opInfo, const std::string& argName, const Value& value) = 0;

    virtual Value putToArgumentViaConnection(const Value& conInfo, const Value& value) = 0;
    
    virtual Value createResource(const std::string& path, const Value& value) = 0;

    virtual Value readResource(const std::string& path) const = 0;

    virtual Value updateResource(const std::string& path, const Value& value) = 0;

    virtual Value deleteResource(const std::string& path) = 0;

    static std::shared_ptr<BrokerAPI> null;
  };

class NullBroker : public BrokerAPI {
  private:

  public:
    NullBroker() : BrokerAPI() {}

    virtual ~NullBroker() {}

    virtual Value getBrokerInfo() const {
      return Value::error("BrokerAPI::getBrokerInfo() failed. Broker is null.");
    }

    virtual Value getProcessInfo() const {
      return Value::error("BrokerAPI::getProcessInfo() failed. Broker is null.");
    }

    virtual Value getOperationInfos() const {
      return Value::error("BrokerAPI::getOperationInfos() failed. Broker is null.");
    }

    virtual Value getOperationInfo(const Value& name) const {
      return Value::error("BrokerAPI::getOperationInfo() failed. Broker is null.");
    }

    virtual Value getContainerInfos() const {
      return Value::error("BrokerAPI::getContainerInfos() failed. Broker is null.");
    }
    
    virtual Value getContainerInfo(const Value& value) const {
      return Value::error("BrokerAPI::getContainerInfo() failed. Broker is null.");
    }

    virtual Value getContainerOperationInfos(const Value& value) const {
      return Value::error("BrokerAPI::getContainerOperationInfos() failed. Broker is null.");
    }

    virtual Value getContainerOperationInfo(const Value& cinfo, const Value& oinfo) const  {
      return Value::error("BrokerAPI::getContainerOperationInfo() failed. Broker is null.");
    }

    virtual Value callContainerOperation(const Value& cinfo, const Value& oinfo, Value&& arg) const {
      return Value::error("BrokerAPI::callContainerOperation() failed. Broker is null.");
    }

    virtual Value invokeContainerOperation(const Value& cinfo, const Value& oinfo) const {
      return Value::error("BrokerAPI::invokeContainerOperation() failed. Broker is null.");
    }

    virtual Value callOperation(const Value& info, Value&& value) const {
      return Value::error("BrokerAPI::callOperation() failed. Broker is null.");
    }

    virtual Value invokeOperation(const Value& name) const {
      return Value::error("BrokerAPI::invokeOperation() failed. Broker is null.");
    }

    virtual Value executeOperation(const Value& info) {
      return Value::error("BrokerAPI::executeOperation() failed. Broker is null.");
    }

    virtual Value getConnectionInfos() const {
      return Value::error("BrokerAPI::getConnectionInfos() failed. Broker is null.");
    }

    virtual Value registerConsumerConnection(const Value& ci) {
      return Value::error("BrokerAPI::registerConsumerConnection() failed. Broker is null.");
    }

    virtual Value registerProviderConnection(const Value& ci) {
      return Value::error("BrokerAPI::registerProviderConnection() failed. Broker is null.");
    }

    virtual Value removeProviderConnection(const Value& ci) {
      return Value::error("BrokerAPI::removeProviderConnection() failed. Broker is null.");
    }

    virtual Value removeConsumerConnection(const Value& ci) {
      return Value::error("BrokerAPI::removeConsumerConnection() failed. Broker is null.");
    }

    virtual Value putToArgument(const Value& opInfo, const std::string& argName, const Value& value) {
      return Value::error("BrokerAPI::putToArgument() failed. Broker is null.");
    }

    virtual Value putToArgumentViaConnection(const Value& conInfo, const Value& value) {
      return Value::error("BrokerAPI::putToArgumentViaConnection() failed. Broker is null.");
    }
    
    virtual Value createResource(const std::string& path, const Value& value) {
      return Value::error("BrokerAPI::createResource() failed. Broker is null.");
    }

    virtual Value readResource(const std::string& path) const {
      return Value::error("BrokerAPI::readResource() failed. Broker is null.");
    }

    virtual Value updateResource(const std::string& path, const Value& value) {
      return Value::error("BrokerAPI::updateResource() failed. Broker is null.");
    }

    virtual Value deleteResource(const std::string& path) {
      return Value::error("BrokerAPI::deleteResource() failed. Broker is null.");
    }
  };

  
}