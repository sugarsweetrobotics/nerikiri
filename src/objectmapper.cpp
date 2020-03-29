#include <regex>

#include "nerikiri/objectmapper.h"
#include "nerikiri/logger.h"
#include "nerikiri/process_store.h"

using namespace nerikiri;


Value ObjectMapper::requestResource(nerikiri::ProcessStore* store, const std::string& path) {

    std::smatch match;

    if (path == "/process/info/") {
        return store->info();
    }
    if (path == "/process/operations/") {
        return store->getOperationInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/"))) {
        return store->getOperation({{"name", Value(match[1])}}).info();
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/invoke/"))) {
        return nerikiri::invoke_operation(store->getOperation({{"name", Value(match[1])}}));
    }
    if (std::regex_match(path, match, std::regex("/process/operations/([^/]*)/connections/"))) {
        return store->getOperation({{"name", Value(match[1])}}).getConnectionInfos();
    }


    if (path == "/process/containers/") {
        return store->getContainerInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/"))) {
        return store->getContainerByName(match[1]).info();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/"))) {
        return store->getContainerByName(match[1]).getOperationInfos();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/"))) {
        return store->getContainerByName(match[1]).getOperation({{"name", Value(match[2])}}).info();
    }
    if (std::regex_match(path, match, std::regex("/process/containers/([^/]*)/operations/([^/]*)/invoke/"))) {
        return nerikiri::invoke_operation(store->getContainerByName(match[1]).getOperation({{"name", Value(match[2])}}));
    }
    return Value::error(logger::error("ObjectMapper::requestResource({}) failed.", path));
}


Value ObjectMapper::createResource(ProcessStore* store, const std::string& path, const Value& value) {

    std::smatch match;
    if (path == "/process/connections/") {
        return store->getOperationInfos();
    }


   return Value::error(logger::error("ObjectMapper::createResource({}) failed.", path));
}