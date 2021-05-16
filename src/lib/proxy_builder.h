#pragma once

#include <juiz/value.h>
#include <juiz/operation_api.h>
#include <juiz/process_store.h>

namespace juiz {



    class ProxyBuilder {
    public:
    static std::shared_ptr<OperationAPI> operationProxy(const Value& info, ProcessStore* store);
    static std::shared_ptr<OperationAPI> operationProxy(const Value& info, const std::shared_ptr<ClientProxyAPI>& brokerProxy);

    static std::shared_ptr<ContainerAPI> containerProxy(const Value& info, ProcessStore* store);
    static std::shared_ptr<ContainerAPI> containerProxy(const Value& info, const std::shared_ptr<ClientProxyAPI>& brokerProxy);

    //static std::shared_ptr<ECStateAPI> ecStateProxy(const Value& info, ProcessStore* store);
    //static std::shared_ptr<ECStateAPI> ecStateProxy(const Value& info, const std::shared_ptr<BrokerProxyAPI>& brokerProxy);

    //static std::shared_ptr<FSMAPI> fsmProxy(const Value& info, ProcessStore* store);
    // static std::shared_ptr<FSMAPI> fsmProxy(const Value& info, const std::shared_ptr<BrokerProxyAPI>& brokerProxy);

    static std::shared_ptr<ConnectionAPI> outgoingOperationConnectionProxy(const Value& info, ProcessStore* store);
    static std::shared_ptr<ConnectionAPI> incomingOperationConnectionProxy(const Value& value, ProcessStore* store);
    };
}