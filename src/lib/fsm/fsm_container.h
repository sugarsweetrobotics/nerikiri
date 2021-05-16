#pragma once

#include <juiz/process_store.h>
namespace juiz {
    bool setupFSMContainer(ProcessStore& store);

    Value createFSM(ProcessStore& store, const std::string& fullName, const Value& fsmInfo);
}