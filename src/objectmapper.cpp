#include <regex>

#include "nerikiri/util/logger.h"
#include "nerikiri/objectmapper.h"
#include "nerikiri/process.h"
#include "nerikiri/process_store.h"

#include "nerikiri/connectionbuilder.h"

using namespace nerikiri;


Value ObjectMapper::readResource(nerikiri::ProcessStore* store, const std::string& path) {

    std::smatch match;

    if (path == "/process/info/") {
        return store->info();
    }
    if (path == "/process/operations/") {
        return store->getOperationInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/info/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}})->info();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}})->invoke();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/connections/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}})->getConnectionInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/connections/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}})->getInputConnectionInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}})->getInputConnectionInfo(match[2]);
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/([^/]*)/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}})->getInputConnectionInfo(match[2], {{"name", Value(match[3])}});
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/output/connections/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}})->getOutputConnectionInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/output/connections/([^/]*)/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}})->getOutputConnectionInfo({{"name", Value(match[2])}});
    }

    if (path == "/process/containers/") {
        return store->getContainerInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/"))) {
        return store->getContainer({{"instanceName", Value(match[1])}})->info();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/"))) {
        return store->getContainer({{"instanceName", Value(match[1])}})->getOperationInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/info/"))) {
        return store->getContainer({{"instanceName", Value(match[1])}})->getOperation({{"instanceName", Value(match[2])}})->info();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/"))) {
        return store->getContainer({{"instanceName", Value(match[1])}})->getOperation({{"instanceName", Value(match[2])}})->invoke();
    }
        if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/connections/"))) {
        return store->getContainer({{"instanceName", Value(match[1])}})->getOperation({{"instanceName", Value(match[2])}})->getConnectionInfos();
    }

    if (path == "/process/connections/") {
      return store->getConnectionInfos();
    }

    if (path == "/process/ecfactories/") {
        return store->getExecutionContextFactoryInfos();
    }
    if (path == "/process/ecs/") {
        return store->getExecutionContextInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/"))) {
        return store->getExecutionContext({{"instanceName", Value(match[1])}})->info();
    }
    if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/state/"))) {
        return store->getExecutionContext({{"instanceName", Value(match[1])}})->info().at("state");
    }
    if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/operations/"))) {
        return store->getExecutionContext({{"instanceName", Value(match[1])}})->getBoundOperationInfos();
    }


    if (path == "/process/brokers/") {
      return store->getBrokerInfos();
    }

    return Value::error(logger::error("ObjectMapper::requestResource({}) failed.", path));
}


Value ObjectMapper::createResource(ProcessStore* store, const std::string& path, const Value& value, BrokerAPI* receiverBroker) {
  std::smatch match;
//  auto store = process->store();
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/"))) {
    //return process->registerConsumerConnection(value);
    return ConnectionBuilder::registerConsumerConnection(store, value);
  }
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/output/connections/"))) {
    //return process->registerProviderConnection(value, receiverBroker);
    return ConnectionBuilder::registerProviderConnection(store, value, receiverBroker);
  }

  if (path == "/process/ecs/") {
    //return process->createExecutionContext(value);
    return ObjectFactory::createExecutionContext(*store, value);
  }

  if (path == "/process/operations/") {
    //return process->createOperation(value);
    return ObjectFactory::createOperation(*store, value);
  }

  if (path == "/process/containers/") {
    //return process->createContainer(value);
    return ObjectFactory::createContainer(*store, value);
  }

  if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/state/"))) {
    if (value.stringValue() == "started") {
        return store->getExecutionContext({{"instanceName", Value(match[1])}})->start();
    } else if (value.stringValue() == "stopped") {
        return store->getExecutionContext({{"instanceName", Value(match[1])}})->stop();
    }
  }

  if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/operations/"))) {
    return store->getExecutionContext({{"instanceName", {match[1]}}})->bind(store->getOperation(value));
  }

  return Value::error(logger::error("ObjectMapper::createResource({}) failed.", path));
}

Value ObjectMapper::updateResource(ProcessStore* store, const std::string& path, const Value& value, BrokerAPI* receiverBroker) {
  std::smatch match;
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/execution/"))) {
    //return proc->executeOperation({{"instanceName", Value(match[1])}});
    return store->getOperation({{"instanceName", Value(match[1])}})->execute();
  }
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/"))) {
    //return proc->putToArgument({{"instanceName", Value(match[1])}}, match[2], value);
    return store->getOperation({{"instanceName", Value(match[1])}})->putToArgument(match[2], value);
  }
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/([^/]*)/"))) {
    if (value.isError()) {
      logger::error("ObjectMapper::updateResource() error. ({})", value);
      return value;
    }
    /*return proc->putToArgumentViaConnection({
        {"input", {
            {"info", {{"instanceName", Value(match[1])}} },
            {"target", {{"name", Value(match[2])}} },
        }},
        {"name", Value(match[3])} }, value);
    */
    return store->getOperation({{"instanceName", Value(match[1])}})->putToArgumentViaConnection({
        {"input", {
            {"info", {{"instanceName", Value(match[1])}} },
            {"target", {{"name", Value(match[2])}} },
        }},
        {"name", Value(match[3])} 
        }, value);  
  }
  if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/"))) {
    return store->getContainer({{"instanceName", Value(match[1])}})->getOperation({{"instanceName", Value(match[2])}})->call(value);
  }



  return Value::error(logger::error("ObjectMapper::updateResource({}) failed.", path));
}

Value ObjectMapper::deleteResource(ProcessStore* store, const std::string& path, BrokerAPI* receiverBroker) {
  std::smatch match;
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/([^/]*)/"))) {
    return ConnectionBuilder::deleteConsumerConnection(store, {
        {"name", Value(match[3])},
        {"input", {
            {"target", {
                {"name", Value(match[2])}
            }},
            {"info", {
                {"instanceName", Value(match[1])}
            }}
        }}
    });
  }
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/output/connections/([^/]*)/"))) {
    return ConnectionBuilder::deleteProviderConnection(store, {
        {"name", Value(match[2])},
        {"output", {
            {"info", {
                {"instanceName", Value(match[1])}
            }}
        }}
    });
  }



  return Value::error(logger::error("ObjectMapper::deleteResource({}) failed.", path));
}
