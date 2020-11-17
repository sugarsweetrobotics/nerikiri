#include <nerikiri/proxy_builder.h>


#include <nerikiri/operation_proxy.h>
using namespace nerikiri;


std::shared_ptr<OperationAPI> ProxyBuilder::operationProxy(const nerikiri::Value& value, nerikiri::ProcessStore* store) {
    auto op = store->operation(Value::string(value.at("fullName")));
    auto brk = store->brokerFactory(Value::string(value.at("broker")))->createProxy(value.at("broker").at("param"));
    return std::make_shared<OperationProxy>(brk, op->fullName());
}