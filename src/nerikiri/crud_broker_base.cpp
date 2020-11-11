#include <nerikiri/crud_broker.h>

#include <nerikiri/process.h>

using namespace nerikiri;

Value CRUDBrokerBase::onCreate(Process* process, const std::string& fullPath, const Value& value) {
    return response([this, process, value](){
        return process->coreBroker()->createResource(req.matches[0], value);
    });
}

Value CRUDBrokerBase::onRead(Process* process, const std::string& fullPath) {
    return response([process, &fullPath](){
        return process->coreBroker()->readResource(fullPath);
    });
}

Value CRUDBrokerBase::onUpdate(Process* process, const std::string& fullPath, const Value& value) {
    return response([this, process, value](){
        return process->coreBroker()->updateResource(req.matches[0], value);
    });
}

Value CRUDBrokerBase::onDelete(Process* process, const std::string& fullPath) {
    return response([process, &fullPath](){
        return process->coreBroker()->deleteResource(fullPath);
    });
}


bool CRUDBroker::run(Process* process) {
    logger::trace("Broker::run()");
    info_["state"] = "running";
    return true;
}

void CRUDBroker::shutdown(Process* process) {
    logger::trace("Broker::shutdown()");
    info_["state"] = "stopped";
}

bool CRUDBroker::isRunning() const { 
    return info_.at("state").stringValue() == "running"; 
}
