#include <nerikiri/crud_broker.h>

#include "object_mapper.h"
#include <nerikiri/process_api.h>

using namespace nerikiri;

Value CRUDBrokerBase::onCreate(ProcessAPI* process, const std::string& fullPath, const Value& value, const Value& ancillaryInfo) {
    return ObjectMapper::createResource(process->coreBroker().get(), fullPath, value, ancillaryInfo, this);
}

Value CRUDBrokerBase::onRead(ProcessAPI* process, const std::string& fullPath, const Value& ancillaryInfo, const Value& brokerInfo) {
    return ObjectMapper::readResource(process->coreBroker().get(), fullPath, ancillaryInfo, brokerInfo);
}

Value CRUDBrokerBase::onUpdate(ProcessAPI* process, const std::string& fullPath, const Value& value, const Value& ancillaryInfo) {
    return ObjectMapper::updateResource(process->coreBroker().get(), fullPath, value, ancillaryInfo, this);
}


Value CRUDBrokerBase::onDelete(ProcessAPI* process, const std::string& fullPath, const Value& ancillaryInfo) {
    return ObjectMapper::deleteResource(process->coreBroker().get(), fullPath, ancillaryInfo, this);
}

bool CRUDBrokerBase::run(ProcessAPI* process) {
    logger::trace("Broker::run()");
    info_["state"] = "running";
    return true;
}

void CRUDBrokerBase::shutdown(ProcessAPI* process) {
    logger::trace("Broker::shutdown()");
    info_["state"] = "stopped";
}

bool CRUDBrokerBase::isRunning() const { 
    return info_.at("state").stringValue() == "running"; 
}
