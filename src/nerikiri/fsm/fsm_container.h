#pragma once

#include <nerikiri/process_store.h>
namespace nerikiri {
    bool setupFSMContainer(ProcessStore& store);

    Value createFSM(ProcessStore& store, const std::string& fullName, const Value& fsmInfo);
}