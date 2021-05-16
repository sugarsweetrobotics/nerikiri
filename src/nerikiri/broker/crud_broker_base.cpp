#include <juiz/crud_broker.h>
#include <juiz/process_api.h>
#include "object_mapper.h"

using namespace nerikiri;

Value CRUDBrokerBase::onCreate(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullPath, const Value& value, const Value& ancillaryInfo) {
    return ObjectMapper::createResource(broker, fullPath, value, ancillaryInfo, this);
}

Value CRUDBrokerBase::onRead(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullPath, const Value& ancillaryInfo, const Value& brokerInfo) {
    return ObjectMapper::readResource(broker, fullPath, ancillaryInfo, brokerInfo);
}

Value CRUDBrokerBase::onUpdate(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullPath, const Value& value, const Value& ancillaryInfo) {
    return ObjectMapper::updateResource(broker, fullPath, value, ancillaryInfo, this);
}


Value CRUDBrokerBase::onDelete(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& fullPath, const Value& ancillaryInfo) {
    return ObjectMapper::deleteResource(broker, fullPath, ancillaryInfo, this);
}

bool CRUDBrokerBase::run(const std::shared_ptr<ClientProxyAPI>& corerBroker) {
    logger::trace("Broker::run()");
    info_["state"] = "running";
    return true;
}

void CRUDBrokerBase::shutdown(const std::shared_ptr<ClientProxyAPI>& corerBroker) {
    logger::trace("Broker::shutdown()");
    info_["state"] = "stopped";
}

bool CRUDBrokerBase::isRunning() const { 
    return info_.at("state").stringValue() == "running"; 
}
