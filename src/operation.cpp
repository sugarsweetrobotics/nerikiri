#include "nerikiri/util/logger.h"
#include "nerikiri/util/functional.h"
#include "nerikiri/operation.h"

using namespace nerikiri;

std::shared_ptr<OperationBase> OperationBase::null = std::make_shared<OperationBase>();



OperationBase::OperationBase(const OperationBase& op): process_(op.process_), Object(op.info_),
  outputConnectionList_(op.outputConnectionList_), 
  inputConnectionListDictionary_(op.inputConnectionListDictionary_), bufferMap_(op.bufferMap_) {
    logger::trace("OperationBase copy construction."); 
    if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
    }
}


Value OperationBase::addProviderConnection(Connection&& c) {
    logger::trace("OperationBase::addProviderConnection({})", str(c.info()));
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    if (c.isNull()) {
        return Value::error(logger::error("addConsumerConnection failed. Given connection is null."));
    }
    outputConnectionList_.emplace_back(std::move(c));
    return c.info();
}

Value OperationBase::addConsumerConnection(Connection&& c) {
    logger::trace("Operation::addConsumerConnection({})", str(c.info()));
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    if (c.isNull()) {
        return Value::error(logger::error("addConsumerConnection failed. Given connection is null."));
    }

    // もし指定されたインスタンス名が自身のインスタンス名と違ったら・・・
    if (c.info()["input"]["info"]["instanceName"] != info().at("instanceName")) {
        if ( (info().hasKey("ownerContainerInstanceName")) ) {
            if (c.info()["input"]["info"]["instanceName"].stringValue() !=
               info().at("ownerContainerInstanceName").stringValue() + ":" + info().at("instanceName").stringValue()) {
                return Value::error(logger::error("Operation::addConsumerConnection failed. Requested connection ({}) 's instanceName does not match to this operation.", c.info()));
            }
        }
    }

    const auto argumentName = c.info()["input"]["target"]["name"].stringValue();
    if (info().at("defaultArg").hasKey(argumentName)) {
        auto inf = c.info();
        inputConnectionListDictionary_[argumentName].emplace_back(std::move(c));
        return inf;
    }
    logger::error("Argument can not found ({}) ", str(c.info()));
    return Value::error("Argument can not found");
}

/*
Value OperationBase::collectValues() const {
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    return Value(info().at("defaultArg").template object_map<std::pair<std::string, Value>>(
    [this](const std::string& key, const Value& value) -> std::pair<std::string, Value>{
        if (!bufferMap_.at(key)->isEmpty()) {
            return {key, bufferMap_.at(key)->pop()};
        }
        for(auto& con : getInputConnectionsByArgName(key)) {
            if (con.isPull()) { return {key, con.pull()}; }
        }
        return {key, value};
    }
    ));
}
*/

/*
Value OperationBase::execute() {
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    logger::trace("OperationBase({})::execute()", str(info()));
    auto v = this->invoke();
    for(auto& c : outputConnectionList_) {
        c.putToArgumentViaConnection(v);
    }
    return v;
}
*/

Value OperationBase::getConnectionInfos() const {
    return {
        {"output", getOutputConnectionInfos()},
        {"input",  getInputConnectionInfos()}
    };
}

Value OperationBase::getInputConnectionInfos() const {
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    return nerikiri::map<std::pair<std::string, Value>, std::string, nerikiri::ConnectionList>(inputConnectionListDictionary_, [](const auto& k, const ConnectionList& v) -> std::pair<std::string,Value> {
        return {k, nerikiri::map<Value, nerikiri::Connection>(v, [](const Connection& con) -> Value { return con.info(); })};
    });
}

Connection OperationBase::getInputConnection(const Value& conInfo) const {
    if (inputConnectionListDictionary_.count(conInfo.at("input").at("target").at("name").stringValue()) == 0) {
        return Connection::null;
    }

    return nerikiri::find_first<Connection, nerikiri::Connection>(inputConnectionListDictionary_.at(conInfo.at("input").at("target").at("name").stringValue()), 
        [&conInfo](const Connection& con) -> bool { return con.info().at("name") == conInfo.at("name"); },
        [](const auto& con) { return con; },
        Connection::null);
}

Value OperationBase::getInputConnectionInfo(const std::string& arg) const {
    if (inputConnectionListDictionary_.count(arg) == 0) {
        return Value::error(logger::error("Operation::getInputConnectionInfo({}) failed. No key.", arg));
    }
    return nerikiri::map<Value, nerikiri::Connection>(inputConnectionListDictionary_.at(arg), 
        [](const Connection& con) -> Value { return con.info(); });
}

Value OperationBase::getInputConnectionInfo(const std::string& arg, const Value& ci) const {
    auto ret = getInputConnectionInfo(arg);
    if (ret.isError()) return ret;
    return nerikiri::find_first<Value, Value>(ret.listValue(),
        [&ci](const auto& v) { return v.at("name") == ci.at("name"); },
        [](const auto& v) { return v; },
        Value::error("Can not find in getInputConnectionInfo(str, value)")
        );
}
/*
ConnectionList OperationBase::getInputConnectionsByArgName(const std::string& argName) const {
    if (inputConnectionListDictionary_.count(argName) == 0) return ConnectionList();
    return inputConnectionListDictionary_.at(argName);
}
*/
Value OperationBase::getOutput() {
    return outputBuffer_.pop();
}

Connection OperationBase::getOutputConnection(const Value& ci) const {
    auto olist = getOutputConnectionList();
    for(auto it = olist.begin();it != olist.end();++it) {
        if ((*it).info().at("name") == ci.at("name")) {
            return (*it);
        }
    }
    return Connection::null;
}

ConnectionList OperationBase::getOutputConnectionList() const {
    return outputConnectionList_;
}

Value OperationBase::getOutputConnectionInfos() const {
    return nerikiri::map<Value, nerikiri::Connection>(getOutputConnectionList(), [](const auto& con) { return con.info(); });
}

Value OperationBase::getOutputConnectionInfo(const Value& ci) const {
    return nerikiri::find_first<Value, nerikiri::Connection>(getOutputConnectionList(), 
        [&ci](const auto& con) -> bool { return con.info().at("name") == ci.at("name"); },
        [](const auto& con) { return con.info(); },
        Value::error("Output Connection can not be found."));
}

bool OperationBase::hasInputConnectionRoute(const ConnectionInfo& ci) const {
    for(const auto v : getInputConnectionsByArgName(ci.at("input").at("target").at("name").stringValue())) {
    if (v.info().at("output") == ci.at("output")) 
        return true;
    }
    return false;
}

bool OperationBase::hasInputConnectionName(const ConnectionInfo& ci) const {
    for(const auto v : getInputConnectionsByArgName(ci.at("input").at("target").at("name").stringValue())) {
    if (v.info().at("name") == ci.at("name")) 
        return true;
    }
    return false;
}

bool OperationBase::hasOutputConnectionRoute(const ConnectionInfo& ci) const {
    for(const auto v : outputConnectionList_) {
    if (v.info().at("input") == ci.at("input")) return true;
    }
    return false;
}

bool OperationBase::hasOutputConnectionName(const ConnectionInfo& ci) const {
    for(const auto v : outputConnectionList_) {
    if (v.info().at("name") == ci.at("name")) return true;
    }
    return false;
}
/*
Value OperationBase::invoke() {
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    return call(collectValues());
}
*/

Value OperationBase::push(const Value& ci, Value&& value) {
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    for (auto& c : inputConnectionListDictionary_[ci.at("target").at("name").stringValue()]) {
        if (c.info().at("name") == ci.at("name")) {
            return bufferMap_[ci.at("target").at("name").stringValue()]->push(std::move(value));
        }
    }
    return Value::error(logger::error("OperationBase::push(Value) can not found connection ({})", str(ci)));
}

Value OperationBase::putToArgument(const std::string& argName, const Value& value) {
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    if (bufferMap_.count(argName) > 0) {
        bufferMap_[argName]->push(value);
        return value;
    }
    return Value::error(logger::error("OperationBaseBase::putToArgument({}) failed.", argName));
}

Value OperationBase::putToArgumentViaConnection(const Value& conInfo, const Value& value) {
    logger::trace("OperationBaseBase::putToArgumentViaConnection()");
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    const std::string& argName = conInfo.at("input").at("target").at("name").stringValue();
    const std::string& conName = conInfo.at("name").stringValue();
    auto connection = this->getInputConnection(conInfo);
    if (bufferMap_.count(argName) > 0) {
        bufferMap_[argName]->push(value);
        if (connection.isEvent()) {
            execute();
        }
        return value;
    }
    return Value::error(logger::error("OperationBaseBase::putToArgument({}) failed.", argName));
}

Value OperationBase::removeProviderConnection(const ConnectionInfo& ci) {
    logger::trace("Operation::removeProviderConnection({})", str(ci));
    if (isNull()) return Value::error(logger::error("Operation::removeProviderConnection failed. Operation is null"));
    //auto olist = getOutputConnectionList();
    for(auto it = this->outputConnectionList_.begin();it != outputConnectionList_.end();) {
    if ((*it).info().at("name") == ci.at("name")) {
        it = outputConnectionList_.erase(it);
    } else { ++ it; }
    }
    return ci;
}

Value OperationBase::removeConsumerConnection(const ConnectionInfo& ci) {
    logger::trace("Operation::removeConsumerConnection({})", str(ci));
    if (isNull()) return Value::error(logger::error("Operation::removeConsumerConnection failed. Operation is null"));
    const auto argName = ci.at("input").at("target").at("name").stringValue();
    for (auto it = inputConnectionListDictionary_[argName].begin(); it != inputConnectionListDictionary_[argName].end();) {
        if ((*it).info().at("name") == ci.at("name")) {
            it = inputConnectionListDictionary_[argName].erase(it);
        } else { ++it; }
    }
    return ci;
}





