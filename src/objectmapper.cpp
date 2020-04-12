#include <regex>

#include "nerikiri/objectmapper.h"
#include "nerikiri/logger.h"
#include "nerikiri/process.h"
#include "nerikiri/process_store.h"

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
        return store->getOperation({{"instanceName", Value(match[1])}}).info();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}}).invoke();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/connections/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}}).getConnectionInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/connections/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}}).getInputConnectionInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}}).getInputConnectionInfo(match[2]);
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/([^/]*)/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}}).getInputConnectionInfo(match[2], {{"name", Value(match[3])}});
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/output/connections/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}}).getOutputConnectionInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/output/connections/([^/]*)/"))) {
        return store->getOperation({{"instanceName", Value(match[1])}}).getOutputConnectionInfo({{"name", Value(match[2])}});
    }

    if (path == "/process/containers/") {
        return store->getContainerInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/"))) {
        return store->getContainer({{"instanceName", Value(match[1])}}).info();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/"))) {
        return store->getContainer({{"instanceName", Value(match[1])}}).getOperationInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/info/"))) {
        return store->getContainer({{"instanceName", Value(match[1])}}).getOperation({{"instanceName", Value(match[2])}}).info();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/"))) {
        return store->getContainer({{"instanceName", Value(match[1])}}).getOperation({{"instanceName", Value(match[2])}}).invoke();
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
    return Value::error(logger::error("ObjectMapper::requestResource({}) failed.", path));
}


Value ObjectMapper::createResource(Process* process, const std::string& path, const Value& value, BrokerAPI* receiverBroker) {
  std::smatch match;
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/"))) {
    return process->registerConsumerConnection(value);
  }
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/output/connections/"))) {
    return process->registerProviderConnection(value, receiverBroker);
  }

  if (path == "/process/ecs/") {
    return process->createExecutionContext(value);
  }

  if (path == "/process/operations/") {
    return process->createOperation(value);
  }

  if (path == "/process/containers/") {
    return process->createContainer(value);
  }

  if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/state/"))) {
    if (value.stringValue() == "started") {
        return process->store()->getExecutionContext({{"instanceName", Value(match[1])}})->start();
    } else if (value.stringValue() == "stopped") {
        return process->store()->getExecutionContext({{"instanceName", Value(match[1])}})->stop();
    }
  }

  if (std::regex_match(path, match, std::regex("/process/ecs/([^/]*)/operations/"))) {
    return process->bindECtoOperation(match[1], value);
  }

  return Value::error(logger::error("ObjectMapper::createResource({}) failed.", path));
}

Value ObjectMapper::updateResource(Process* proc, const std::string& path, const Value& value, BrokerAPI* receiverBroker) {
  std::smatch match;

  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/"))) {
    return proc->putToArgument({{"instanceName", Value(match[1])}}, match[2], value);
  }
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/([^/]*)/"))) {
    return proc->putToArgumentViaConnection({
        {"input", {
            {"info", {{"instanceName", Value(match[1])}} },
            {"target", {{"name", Value(match[2])}} },
        }},
        {"name", Value(match[3])} }, value);
  }
  if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/"))) {
    return proc->getContainer({{"instanceName", Value(match[1])}}).getOperation({{"instanceName", Value(match[2])}}).call(value);
  }
  return Value::error(logger::error("ObjectMapper::writeResource({}) failed.", path));
}

Value ObjectMapper::deleteResource(Process* process, const std::string& path, BrokerAPI* receiverBroker) {
  std::smatch match;
  if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/input/arguments/([^/]*)/connections/([^/]*)/"))) {
    return process->deleteConsumerConnection({
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
    return process->deleteProviderConnection({
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
