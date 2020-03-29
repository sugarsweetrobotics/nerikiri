#pragma once

#include "nerikiri/value.h"

namespace nerikiri {

    class Process;
    class ProcessStore;

  class BrokerAPI {
  private:

  public:
    BrokerAPI() {}

    virtual ~BrokerAPI() {}

    virtual bool run() = 0;
    
    virtual void shutdown() = 0;

    virtual void setProcess(Process* process)  = 0;

    virtual void setProcessStore(ProcessStore* store)  = 0;

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

    virtual Value makeConnection(const Value& ci) const = 0;

    virtual Value getConnectionInfos() const = 0;

    virtual Value registerConsumerConnection(const Value& ci) const = 0;

    virtual Value removeConsumerConnection(const Value& ci) const = 0;

    virtual Value pushViaConnection(const Value& ci, Value&& value)  const = 0;

    virtual Value requestResource(const std::string& path) const = 0;

    virtual Value createResource(const std::string& path, const Value& value) = 0;
  };
}