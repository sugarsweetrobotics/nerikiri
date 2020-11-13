#include <nerikiri/crud_broker.h>

#include <nerikiri/object_mapper.h>
#include <nerikiri/process.h>

using namespace nerikiri;

Value CRUDBrokerBase::onCreate(Process* process, const std::string& fullPath, const Value& value) {
    return ObjectMapper::createResource(process->coreBroker().get(), fullPath, value);
}

Value CRUDBrokerBase::onRead(Process* process, const std::string& fullPath) {
    return ObjectMapper::readResource(process->coreBroker().get(), fullPath);
}

Value CRUDBrokerBase::onUpdate(Process* process, const std::string& fullPath, const Value& value) {
    return ObjectMapper::updateResource(process->coreBroker().get(), fullPath, value);
}


Value CRUDBrokerBase::onDelete(Process* process, const std::string& fullPath) {
    return ObjectMapper::deleteResource(process->coreBroker().get(), fullPath);
}

bool CRUDBrokerBase::run(Process* process) {
    logger::trace("Broker::run()");
    info_["state"] = "running";
    return true;
}

void CRUDBrokerBase::shutdown(Process* process) {
    logger::trace("Broker::shutdown()");
    info_["state"] = "stopped";
}

bool CRUDBrokerBase::isRunning() const { 
    return info_.at("state").stringValue() == "running"; 
}
