#pragma once

#include <nerikiri/process_store.h>
namespace nerikiri {
    bool setupECContainer(ProcessStore& store);

    Value createEC(ProcessStore& store, const std::string& fullName, const Value& ecInfo);
}