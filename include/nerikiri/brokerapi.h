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

        virtual Value getAllOperationInfos()  const = 0;

        virtual Value getOperationInfo(const Value& name) const = 0;

        virtual Value getContainerInfos() const = 0;

        virtual Value getContainerInfo(const Value& value) const = 0;

        virtual Value getContainerOperationInfos(const Value& value) const = 0;

        virtual Value getContainerOperationInfo(const Value& cinfo, const Value& oinfo) const  = 0;

        virtual Value callContainerOperation(const Value& cinfo, const Value& oinfo, Value&& arg) = 0;

        virtual Value invokeContainerOperation(const Value& cinfo, const Value& oinfo) const  = 0;

        virtual Value callOperation(const Value& info, Value&& value) = 0;

        virtual Value invokeOperation(const Value& name) const = 0;

        virtual Value executeOperation(const Value& info) = 0;

        virtual Value getConnectionInfos() const = 0;

        virtual Value registerConsumerConnection(const Value& ci)  = 0;

        virtual Value registerProviderConnection(const Value& ci)  = 0;

        virtual Value removeProviderConnection(const Value& ci) = 0;

        virtual Value removeConsumerConnection(const Value& ci) = 0;

        virtual Value putToArgument(const Value& opInfo, const std::string& argName, const Value& value) = 0;

        virtual Value putToArgumentViaConnection(const Value& conInfo, const Value& value) = 0;

        virtual Value getOperationFactoryInfos() const = 0;

        virtual Value getContainerFactoryInfos() const = 0;

        virtual Value createResource(const std::string& path, const Value& value) = 0;

        virtual Value readResource(const std::string& path) const = 0;

        virtual Value updateResource(const std::string& path, const Value& value) = 0;

        virtual Value deleteResource(const std::string& path) = 0;


        virtual Value createOperation(const Value& value) = 0;

        virtual Value createContainer(const Value& value) = 0;

        virtual Value createContainerOperation(const Value& containerInfo, const Value& value) = 0;

        virtual Value deleteOperation(const Value& value) = 0;

        virtual Value deleteContainer(const Value& value) = 0;

        virtual Value deleteContainerOperation(const Value& containerInfo, const Value& value) = 0;

        virtual Value createExecutionContext(const Value& value) = 0;

        virtual Value deleteExecutionContext(const Value& value) = 0;
    };


}