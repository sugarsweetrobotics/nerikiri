#pragma once

#include <nerikiri/value.h>
#include <nerikiri/operation_api.h>
#include <nerikiri/process_store.h>

namespace nerikiri {



    class ProxyBuilder {
    public:
    static std::shared_ptr<OperationAPI> operationProxy(const Value& info, ProcessStore* store);
    static std::shared_ptr<ConnectionAPI> outgoingConnectionProxy(const Value& info, ProcessStore* store);
    static std::shared_ptr<ConnectionAPI> incomingConnectionProxy(const Value& value, ProcessStore* store);
    };
}