#include <regex>

#include "nerikiri/logger.h"
#include "nerikiri/objectmapper.h"
#include "nerikiri/process.h"
#include "nerikiri/process_store.h"

#include "nerikiri/connectionbuilder.h"

using namespace nerikiri;


Value ObjectMapper::readResource(const std::shared_ptr<CoreBroker>& coreBroker, const std::string& path) {

    std::smatch match;

    if (path == "/process/info/") {
        return coreBroker->getProcessInfo();
    }
    if (path == "/process/operations/") {
        return coreBroker->getOperationInfos();
    }
    if (path == "/process/all_operations/") {
      return coreBroker->getAllOperationInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/info/"))) {
        return coreBroker->getOperationInfo(match[1]);
    }
    if (std::regex_match(path, match, std::regex("/process/all_operations/([^/]*)/info/"))) {
        return coreBroker->getAllOperationInfo(match[1]);
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/"))) {
        return coreBroker->invokeOperation(match[1]);
    }
    if (std::regex_match(path, match, std::regex("/process/all_operations/([^/]*)/"))) {
        return coreBroker->invokeAllOperation(match[1]);
    }

    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/connections/"))) {
        return coreBroker->getOperationConnectionInfos(match[1]);
    }
    if (std::regex_match(path, match, std::regex("/process/all_operations/([^/]*)/connections/"))) {
        return coreBroker->getAllOperationConnectionInfos(match[1]);
    }
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
    if (path == "/process/containers/") {
      return coreBroker->getContainerInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/"))) {
      return coreBroker->getContainerInfo(match[1]);
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/"))) {
      return coreBroker->getContainerOperationInfos(match[1]);
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/info/"))) {
      return coreBroker->getContainerOperationInfos(match[1].str() + ":" + match[2].str());
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/"))) {
      return coreBroker->invokeContainerOperation(match[1].str() + ":" + match[2].str());
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/connections/"))) {
      return coreBroker->getContainerOperationConnectionInfos(match[1].str() + ":" + match[2].str());
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
      return coreBroker->getInvokeConnectionInfos(match[1]);
    }

    return Value::error(logger::error("ObjectMapper::requestResource({}) failed.", path));
}


Value ObjectMapper::createResource(const std::shared_ptr<CoreBroker>& coreBroker, const std::string& path, const Value& value, BrokerAPI* receiverBroker) {
  std::smatch match;
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/"))) {
    return coreBroker->registerConsumerConnection(value);
  }
  if (std::regex_match(path, match, std::regex("/process/connections/"))) {
    return coreBroker->registerProviderConnection(value);
  }
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/output/connections/"))) {
    return coreBroker->registerProviderConnection(value);
  }

  if (path == "/process/ecs/") {
    return coreBroker->createExecutionContext(value);
  }

  if (path == "/process/operations/") {
    return coreBroker->createOperation(value);
  }


  if (path == "/process/containers/") {
    return coreBroker->createContainer(value);
  }

  if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/state/"))) {
    return coreBroker->setExecutionContextState(match[1], value.stringValue());
  }

  if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/operations/([^/]*)/"))) {
    return coreBroker->bindOperationToExecutionContext(match[1], match[2], value);
  }

  return Value::error(logger::error("ObjectMapper::createResource({}) failed.", path));
}

/**
 * 
 * 
 * 
 */
Value ObjectMapper::updateResource(const std::shared_ptr<CoreBroker>& coreBroker, const std::string& path, Value&& value, BrokerAPI* receiverBroker) {
  logger::trace("ObjectMapper::updateResource(store, path={}", path);
  std::smatch match;
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/execution/"))) {
    //logger::trace("In ObjectMapper::updateResource. executing operation(store, {})", value);
    return coreBroker->executeOperation(match[1]);
  }
  if (std::regex_match(path, match, std::regex("/process/all_operations/([^/]*)/execution/"))) {
    //logger::trace("In ObjectMapper::updateResource. executing operation(store, {})", value);
    return coreBroker->executeAllOperation(match[1]);
  }
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/"))) {
    return coreBroker->putToArgument(match[1], match[2], value);
  }
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/([^/]*)/"))) {
    if (value.isError()) {
      logger::error("ObjectMapper::updateResource() error. ({})", value);
      return value;
    }
    return coreBroker->putToArgumentViaConnection(match[1], match[2], match[3], value);
    //return store->getAllOperation({{"fullName", Value(match[1])}})->putToArgumentViaConnection({
    //    {"input", {
    //        {"info", {{"fullName", Value(match[1])}} },
    //        {"target", {{"name", Value(match[2])}} },
    //    }},
    //    {"name", Value(match[3])} 
    //    }, value);  
  }
  if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/"))) {
    coreBroker->callContainerOperation(match[1].str() + ":" + match[2].str(), std::move(value));
    //return store->getContainer({{"fullName", Value(match[1])}})->getOperation({{"instanceName", Value(match[2])}})->call(value);
  }



  return Value::error(logger::error("ObjectMapper::updateResource({}) failed.", path));
}

/**
 * 
 * 
 * 
 * 
 */
Value ObjectMapper::deleteResource(const std::shared_ptr<CoreBroker>& coreBroker, const std::string& path, BrokerAPI* receiverBroker) {
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

  return Value::error(logger::error("ObjectMapper::deleteResource({}) failed.", path));
}
