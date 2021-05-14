#include <regex>

#include <nerikiri/logger.h>
#include "object_mapper.h"
//#include "../process/process_impl.h"
#include <nerikiri/process_api.h>
#include "nerikiri/process_store.h"

// #include "nerikiri/connection_builder.h"

using namespace nerikiri;


Value ObjectMapper::createResource(const std::shared_ptr<ClientProxyAPI>& coreBroker, const std::string& _path, const Value& value, const Value& params, BrokerAPI* receiverBroker) {
  auto path = _path;
    if (path.at(path.length()-1) == '/') { path = path.substr(0, path.length()-1); }


  std::smatch match;
  logger::info("ObjectMapper::createResource({}, {}) called.", path, value);

  if (std::regex_match(path, match, std::regex("([^/]*)s$"))) {
    return coreBroker->factory()->createObject(match[1], value);
  }

  if (path == "/process/operations/") { return coreBroker->factory()->createObject("operation", value); }
  if (path == "/process/containers/") { return coreBroker->factory()->createObject("container", value); }
  if (path == "/process/containerOperations/") { return coreBroker->factory()->createObject("containerOperation", value); }
  if (path == "/process/ecs/") { return coreBroker->factory()->createObject("ec", value); }
  if (path == "/process/fsms/") { return coreBroker->factory()->createObject("fsm", value); }
  if (path == "/process/connections/") { return coreBroker->factory()->createObject("connection", value); } 

  
  if (std::regex_match(path, match, std::regex("operations/([^/]*)/outlet/connections"))) {
    return coreBroker->factory()->createObject("outletConnection", Value::merge(value, {{"operationFullName", match[1].str()}} ));
  }
  if (std::regex_match(path, match, std::regex("operations/([^/]*)/inlets/([^/]*)/connections"))) {
    return coreBroker->factory()->createObject("inletConnection", Value::merge(value, 
                              {{"operationFullName", match[1].str()}, {"inletFullName", match[2].str()} } ));
  }
  if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/operations/"))) {
    return coreBroker->factory()->createObject("ecBind", Value::merge(value, {{"ecFullName", match[1].str()}}));
  }

  return Value::error(logger::error("ObjectMapper::createResource({}) failed. No route matched", path));
}

/**
 * 
 */
Value ObjectMapper::readResource(const std::shared_ptr<const ClientProxyAPI>& coreBroker, const std::string& _path, const Value& params, const Value& receiverBrokerInfo) {
    auto path = _path;
    if (path.length() == 0) 
      return Value::error(logger::error("ObjectMapper::requestResource({}) failed.", path));
    if (path.at(path.length()-1) == '/') { path = path.substr(0, path.length()-1); }

    logger::debug("ObjectMapper::readResource({})", path);
    std::smatch match;
    
    if (path == "info") {
        return coreBroker->getProcessInfo();
    }
    if (path == "fullInfo") {
        return coreBroker->getProcessFullInfo();
    }

    if (std::regex_match(path, match, std::regex("([^/]*)ies$"))) {
      return coreBroker->store()->getClassObjectInfos(match[1].str() + "y");
    } else if (std::regex_match(path, match, std::regex("([^/]*)s$"))) {
      return coreBroker->store()->getClassObjectInfos(match[1]);
    }

    if (std::regex_match(path, match, std::regex("([^/]*)ies/([^/]*)$"))) {
      return coreBroker->store()->getObjectInfo(match[1].str()+"y", match[2]);
    } else if (std::regex_match(path, match, std::regex("([^/]*)s/([^/]*)$"))) {
      return coreBroker->store()->getObjectInfo(match[1], match[2]);
    }

    if (std::regex_match(path, match, std::regex("operations/([^/]*)/inlets$"))) {
      return coreBroker->operation()->inlets(match[1]);
    } else if (std::regex_match(path, match, std::regex("operations/([^/]*)/inlets/([^/]*)$"))) {
      return coreBroker->operationInlet()->get(match[1], match[2]);
    } else if (std::regex_match(path, match, std::regex("operations/([^/]*)/inlets/([^/]*)/info$"))) {
      return coreBroker->operationInlet()->info(match[1], match[2]);
    } else if (std::regex_match(path, match, std::regex("operations/([^/]*)/inlets/([^/]*)/connections$"))) {
      return coreBroker->operationInlet()->connections(match[1], match[2]);
    } else if (std::regex_match(path, match, std::regex("operations/([^/]*)/outlet/info$"))) {
      return coreBroker->operationOutlet()->info(match[1]);
    } else if (std::regex_match(path, match, std::regex("operations/([^/]*)/outlet/connections$"))) {
      return coreBroker->operationOutlet()->connections(match[1]);
    } else if (std::regex_match(path, match, std::regex("operations/([^/]*)/outlet$"))) {
      return coreBroker->operationOutlet()->get(match[1]);
    } else if (std::regex_match(path, match, std::regex("operations/([^/]*)/fullInfo$"))) {
      auto v = coreBroker->operation()->fullInfo(match[1]);
      if (!receiverBrokerInfo.isNull() && !v.isError()) v["broker"] = receiverBrokerInfo;
      return v;
    } 

    if (std::regex_match(path, match, std::regex("containers/([^/]*)/operations$"))) {  
      return coreBroker->container()->operations(match[1]);
    } else if (std::regex_match(path, match, std::regex("containers/([^/]*)/fullInfo$"))) {
      auto v = coreBroker->container()->fullInfo(match[1]);
      if (!receiverBrokerInfo.isNull() && !v.isError()) v["broker"] = receiverBrokerInfo;
      return v;
    } 

    return Value::error(logger::error("ObjectMapper::readResource({}) failed. No route matched", path));
}


/**
 * 
 * 
 * 
 */
Value ObjectMapper::updateResource(const std::shared_ptr<ClientProxyAPI>& coreBroker, const std::string& path, const Value& value, const Value& params, BrokerAPI* receiverBroker) {
  logger::debug("ObjectMapper::updateResource(store, path={}, value={}", path, value);
  std::smatch match;



  if (std::regex_match(path, match, std::regex("operations/([^/]*)/inlets/([^/]*)"))) {
    return coreBroker->operationInlet()->put(match[1], match[2], value);
  }

  if (std::regex_match(path, match, std::regex("operations/([^/]*)/call"))) {
    return coreBroker->operation()->call(match[1].str(), (value));
  }

  if (std::regex_match(path, match, std::regex("operations/([^/]*)/invoke"))) {
    return coreBroker->operation()->invoke(match[1].str());
  }

  if (std::regex_match(path, match, std::regex("operations/([^/]*)/execute"))) {
    return coreBroker->operation()->execute(match[1].str());
  }


  if (std::regex_match(path, match, std::regex("ecs/([^/]*)/state"))) {
    if (Value::string(value) == "started") {
 //      coreBroker->ec()->activateStart(match[1]);
    } else if (Value::string(value) == "stopped") {
 //     coreBroker->ec()->activateStop(match[1]);
    } else {
      return Value::error(logger::error("ObjectMapper::updateResource({}, {}) failed. ObjectMapper tried to update EC's state, but the argument can not be recognized."));
    }
  }
  return Value::error(logger::error("ObjectMapper::updateResource({}) failed. No route matched", path));
}

/**
 * 
 * 
 * 
 * 
 */
Value ObjectMapper::deleteResource(const std::shared_ptr<ClientProxyAPI>& coreBroker, const std::string& _path, const Value& params, BrokerAPI* receiverBroker) {
  auto path = _path;
  if (path.length() == 0) 
    return Value::error(logger::error("ObjectMapper::requestResource({}) failed.", path));
  if (path.at(path.length()-1) == '/') { path = path.substr(0, path.length()-1); }

  logger::debug("ObjectMapper::deleteResource(path={}", path);std::smatch match;
  if (std::regex_match(path, match, std::regex("connections/([^/]*)"))) {
    return coreBroker->connection()->deleteConnection(match[1]);
  }
  return Value::error(logger::error("ObjectMapper::deleteResource({}) failed. No route matched", path));
}
