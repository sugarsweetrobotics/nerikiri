
#include <nerikiri/process.h>
#include <nerikiri/broker_api.h>
#include <nerikiri/operation.h>
#include <nerikiri/connection.h>

using namespace nerikiri;

/*
Broker::Broker() : Object() {}

Broker::Broker(const Value& info) : Object(info) {}

Broker::~Broker() {}

bool Broker::run(Process* process) {
    logger::trace("Broker::run()");
    info_["state"] = "running";
    return true;
}

void Broker::shutdown(Process* process) {
    logger::trace("Broker::shutdown()");
    info_["state"] = "stopped";
}

bool Broker::isRunning() const { 
    return info_.at("state").stringValue() == "running"; 
}

std::shared_ptr<Broker> nerikiri::nullBroker() {
    return std::make_shared<Broker>();
}
*/
