#pragma once

#include <nerikiri/value.h>
#include <nerikiri/operation_api.h>
#include <nerikiri/process_store.h>

namespace nerikiri {



    class ProxyBuilder {
    public:
    static std::shared_ptr<OperationAPI> operationProxy(const Value& info, ProcessStore* store);
    };
}