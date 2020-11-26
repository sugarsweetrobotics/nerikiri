#include <regex>

#include <nerikiri/logger.h>
#include <nerikiri/object_mapper.h>
#include "nerikiri/process.h"
#include "nerikiri/process_store.h"

// #include "nerikiri/connection_builder.h"

using namespace nerikiri;


Value ObjectMapper::createResource(BrokerProxyAPI* coreBroker, const std::string& _path, const Value& value, const std::map<std::string, std::string>& params, BrokerAPI* receiverBroker) {
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

  if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/"))) {
    return coreBroker->factory()->createObject("containerOperation", Value::merge(value, {{"containerFullName", match[1].str()}} ));
  }
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

  return Value::error(logger::error("ObjectMapper::createResource({}) failed. This request could not find appropreate callbacks", path));
}

Value ObjectMapper::readResource(const BrokerProxyAPI* coreBroker, const std::string& _path, const std::map<std::string, std::string>& params) {
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
    } else if (std::regex_match(path, match, std::regex("operations/([^/]*)/outlet$"))) {
      return coreBroker->operationOutlet()->info(match[1]);
    }

    /*

    if (std::regex_match(path, match, std::regex("/process/operations/$"))) { return coreBroker->store()->getClassObjectInfos("operation"); }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/"))) { return coreBroker->store()->getObjectInfo("operation", match[1]); }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/outlet/connections/"))) { return coreBroker->store()->getChildrenClassObjectInfos(match[1], "outletConnection"); }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/outlet/connections/([^/]*)/"))) { return coreBroker->store()->getObjectInfo("outletConnection", nerikiri::naming::join(match[1], match[2])); }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/inlets/([^/]*)/connections/"))) { return coreBroker->store()->getChildrenClassObjectInfos(nerikiri::naming::join(match[1],match[2]), "inletConnection"); }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/inlets/([^/]*)/connections/([^/]*)/"))) { return coreBroker->store()->getObjectInfo("inletConnection", nerikiri::naming::join(match[1], match[2], match[3])); }

    if (path == "/process/containers/") { return coreBroker->store()->getClassObjectInfos("container"); }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/"))) { return coreBroker->store()->getObjectInfo("container", match[1]); }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/"))) { return coreBroker->store()->getChildrenClassObjectInfos(match[1], "containerOperation"); }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/"))) { return coreBroker->store()->getObjectInfo("containerOperation", nerikiri::naming::join(match[1], match[2])); }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/connections/"))) {
      return coreBroker->store()->getChildrenClassObjectInfos(nerikiri::naming::join(match[1], match[2]), "connection"); 
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/connections/([^/]*)/"))) {
      return coreBroker->store()->getObjectInfo("connection", nerikiri::naming::join(match[1], match[2], match[3])); 
    }
    */
    /*
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/connections/"))) {
        return store->getAllOperation({{"fullName", Value(match[1])}})->getInputConnectionInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/"))) {
        return store->getAllOperation({{"fullName", Value(match[1])}})->getInputConnectionInfo(match[2]);
    }
    
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/([^/]*)/"))) {
        return store->getAllOperation({{"fulllName", Value(match[1])}})->getInputConnectionInfo(match[2], {{"name", Value(match[3])}});
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/output/connections/"))) {
        return store->getAllOperation({{"fullName", Value(match[1])}})->getOutputConnectionInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/output/connections/([^/]*)/"))) {
        return store->getAllOperation({{"fullName", Value(match[1])}})->getOutputConnectionInfo({{"name", Value(match[2])}});
    }
    */
    
    
    /*
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/info/"))) {
      return coreBroker->getContainerOperationInfos(match[1].str() + ":" + match[2].str());
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/"))) {
      return coreBroker->invokeContainerOperation(match[1].str() + ":" + match[2].str());
    }
    

  
    if (std::regex_match(path, match, std::regex("/process/fsms/([^/]*)/"))) {
      return coreBroker->getFSMInfo(match[1].str());
    }
    if (std::regex_match(path, match, std::regex("/process/fsms/([^/]*)/state/"))) {
      return coreBroker->getFSMState(match[1].str());
    }
    if (std::regex_match(path, match, std::regex("/process/fsms/([^/]*)/state/([^/]*)/operations/"))) {
      return coreBroker->getFSMState(match[1].str());
    }

    if (path == "/process/connections/") {
      return coreBroker->getConnectionInfos();
    }
    if (path == "/process/ecfactories/") {
        return coreBroker->getExecutionContextFactoryInfos();
    }
    if (path == "/process/ecs/") {
        return coreBroker->getExecutionContextInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/"))) {
        return coreBroker->getExecutionContextInfo(match[1]);
    }
    if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/state/"))) {
        return coreBroker->getExecutionContextState(match[1]);
    }
    if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/operations/"))) {
      return coreBroker->getExecutionContextBoundOperationInfos(match[1]);
    }
    if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/all_operations/"))) {
      return coreBroker->getExecutionContextBoundAllOperationInfos(match[1]);
    }

    if (path == "/process/brokers/") {
      return coreBroker->getBrokerInfos();
    }

    if (path == "/process/callbacks/") {
      return coreBroker->getCallbacks();
    }

    if (path == "/process/operationFactories/") {
      return coreBroker->getOperationFactoryInfos();
    }

    if (path == "/process/containerFactories/") {
      return coreBroker->getContainerFactoryInfos();
    }

    if (path == "/process/topics/") {
      return coreBroker->getTopicInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/topics/([^/]*)/"))) {
      return coreBroker->invokeTopic(match[1]);
    }
    if (std::regex_match(path, match, std::regex("/process/topics/([^/]*)/connections/"))) {
      return coreBroker->getTopicConnectionInfos(match[1]);
    }
    */
    return Value::error(logger::error("ObjectMapper::readResource({}) failed.", path));
}


/**
 * 
 * 
 * 
 */
Value ObjectMapper::updateResource(BrokerProxyAPI* coreBroker, const std::string& path, const Value& value, const std::map<std::string, std::string>& params, BrokerAPI* receiverBroker) {
  logger::debug("ObjectMapper::updateResource(store, path={}, value={}", path, value);
  std::smatch match;

  if (std::regex_match(path, match, std::regex("operations/([^/]*)/inlets/([^/]*)"))) {
    return coreBroker->operationInlet()->put(match[1], match[2], value);
  }


  /*
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/"))) {
    return coreBroker->callOperation(match[1].str(), (value));
  }
  if (std::regex_match(path, match, std::regex("/process/all_operations/([^/]*)/"))) {
    return coreBroker->callAllOperation(match[1].str(), (value));
  }


  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/execution/"))) {
    //logger::trace("In ObjectMapper::updateResource. executing operation(store, {})", value);
    return coreBroker->executeOperation(match[1]);
  }
  if (std::regex_match(path, match, std::regex("/process/all_operations/([^/]*)/execution/"))) {
    //logger::trace("In ObjectMapper::updateResource. executing operation(store, {})", value);
    return coreBroker->executeAllOperation(match[1]);
  }

  
  if (std::regex_match(path, match, std::regex("/process/all_operations/([^/]*)/input/arguments/([^/]*)/connections/([^/]*)/"))) {
    if (value.isError()) {
      logger::error("ObjectMapper::updateResource() error. ({})", value);
      return value;
    }
    return coreBroker->putToArgumentViaConnection(match[1], match[2], match[3], value);
  }

  if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/state/"))) {
    return coreBroker->setExecutionContextState(match[1], value.stringValue());
  }

  if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/"))) {
    return coreBroker->callContainerOperation(match[1].str() + ":" + match[2].str(), (value));
    //return store->getContainer({{"fullName", Value(match[1])}})->getOperation({{"instanceName", Value(match[2])}})->call(value);
  }
  if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/execution/"))) {
    return coreBroker->executeContainerOperation(match[1].str() + ":" + match[2].str());
  }

  if (std::regex_match(path, match, std::regex("/process/fsms/([^/]*)/state/"))) {
    return coreBroker->setFSMState(match[1].str(), getStringValue(value, ""));
  }
  */
  return Value::error(logger::error("ObjectMapper::updateResource({}) failed.", path));
}

/**
 * 
 * 
 * 
 * 
 */
Value ObjectMapper::deleteResource(BrokerProxyAPI* coreBroker, const std::string& path, const std::map<std::string, std::string>& params, BrokerAPI* receiverBroker) {
  logger::debug("ObjectMapper::deleteResource(path={}", path);
  /*
  std::smatch match;
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/([^/]*)/"))) {
    return coreBroker->removeConsumerConnection(match[1], match[2], match[3]);
    //return ConnectionBuilder::deleteConsumerConnection(store, {
    //    {"name", Value(match[3])},
    //    {"input", {
    //        {"target", {
    //            {"name", Value(match[2])}
    //        }},
    //        {"info", {
    //            {"fullName", Value(match[1])}
    //        }}
    //    }}
    //});
  }
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/output/connections/([^/]*)/"))) {
    return coreBroker->removeProviderConnection(match[1], match[2]);
    //return ConnectionBuilder::deleteProviderConnection(store, {
    //    {"name", Value(match[2])},
    //    {"output", {
    //        {"info", {
    //            {"fullName", Value(match[1])}
    //        }}
    //    }}
    //});
  }

  if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/operations/([^/]*)/"))) {
    return coreBroker->unbindOperationFromExecutionContext(match[1], match[2]);
  }
   */
  return Value::error(logger::error("ObjectMapper::deleteResource({}) failed.", path));
}
