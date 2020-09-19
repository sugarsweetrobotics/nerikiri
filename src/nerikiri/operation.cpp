#include <iostream>

#include "nerikiri/logger.h"
#include "nerikiri/functional.h"
#include "nerikiri/operation.h"

using namespace nerikiri;

//std::shared_ptr<OperationBase> OperationBase::null = std::make_shared<OperationBase>();

OperationBase::OperationBase(const Value& info):
    Object(info), argument_updated_(true) {
    //logger::trace("OperationBase::OperationBase({})", str(info));
    if (!operationValidator(info_)) {
        is_null_ = true;//logger::error("OperationInformation is invalid.");
    } else {
    info_.at("defaultArg").object_for_each([this](const std::string& key, const Value& value) -> void{
        inputConnectionListDictionary_.emplace(key, ConnectionList());
        bufferMap_.emplace(key, std::make_shared<NewestValueBuffer>(info_.at("defaultArg").at(key)));
    });
    }
}

OperationBase::OperationBase(const OperationBase& op): /*process_(op.process_),*/ Object(op.info_),
  outputConnectionList_(op.outputConnectionList_), 
  inputConnectionListDictionary_(op.inputConnectionListDictionary_), 
  bufferMap_(op.bufferMap_), argument_updated_(op.argument_updated_) {
    logger::trace("OperationBase copy construction."); 
    if (!operationValidator(info_)) {
        logger::error("OperationInformation is invalid.");
    }
}


Value OperationBase::addProviderConnection(Connection&& c) {
    logger::trace("OperationBase::addProviderConnection({})", str(c.info()));
    if (isNull()) { 
        return Value::error(logger::error("OperationBase::addProviderConnection({}) failed. Caller Operation is null.", c.info())); 
    }
    if (c.isNull()) {
        return Value::error(logger::error("OperationBase::addProviderConnection failed. Given connection is null."));
    }
    outputConnectionList_.emplace_back(std::move(c));
    return c.info();
}

Value OperationBase::addConsumerConnection(Connection&& c) {
    logger::trace("OperationBase::addConsumerConnection({})", str(c.info()));
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    if (c.isNull()) {
        return Value::error(logger::error("OperationBase::addConsumerConnection() failed. Given connection is null."));
    }

    // もし指定されたインスタンス名が自身のインスタンス名と違ったら・・・
    if (c.info()["input"]["info"]["fullName"] != info().at("fullName")) {
        return Value::error(logger::error("OperationBase::addConsumerConnection failed. Requested connection ({}) 's instanceName does not match to this operation.", c.info()));
    }

    const auto argumentName = c.info()["input"]["target"]["name"].stringValue();
    if (info().at("defaultArg").hasKey(argumentName)) {
        auto inf = c.info();
        inputConnectionListDictionary_[argumentName].emplace_back(std::move(c));
        logger::info("OperationBase::addConsumerConnection({}) success.");
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

#if (0)
Value OperationBase::execute() {
    if (isNull()) { return Value::error("OperationBase::execute() failed. Caller Operation is null."); }
    logger::trace(("OperationBase("+this->info().at("instanceName").stringValue()+") invoking").c_str());
    Value&& v = this->invoke();
    logger::trace(("OperationBase("+this->info().at("instanceName").stringValue()+") invoked").c_str());
    for (auto& c : outputConnectionList_) {
        c.putToArgumentViaConnection(v);
    }
    return v;
}
#endif

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

Connection OperationBase::getInputConnection(const std::string& argName, const std::string& conName) const {
    logger::trace("OperationBase::getInputConnection()");
    for(auto cn : inputConnectionListDictionary_) {
        logger::trace(" - {}", cn.first);
    }
    if (inputConnectionListDictionary_.count(argName) == 0) {
        return Connection::null;
    }

    return nerikiri::find_first<Connection, nerikiri::Connection>(inputConnectionListDictionary_.at(argName), 
        [&conName](const Connection& con) -> bool { return con.info().at("name").stringValue() == conName; },
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

Connection OperationBase::getOutputConnection(const std::string& name) const {
    auto olist = getOutputConnectionList();
    for(auto it = olist.begin();it != olist.end();++it) {
        if ((*it).info().at("name").stringValue() == name) {
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

Value OperationBase::collectValues() {
    logger::trace("OperationBase({})::collectValues()", info());
    if (isNull()) { return Value::error(logger::error("OperationBase({})::collectValues() failed. Caller Operation is null.", info())); }

    std::lock_guard<std::mutex> lock(argument_mutex_);
    return Value(info().at("defaultArg").template object_map<std::pair<std::string, Value>>(
        [this](const std::string& key, const Value& value) -> std::pair<std::string, Value> {
        if (!bufferMap_.at(key)->isEmpty()) {
            return { key, bufferMap_.at(key)->popRef() };
        }
        for (auto& con : getInputConnectionsByArgName(key)) {
            if (con.isPull()) { 
                logger::trace(" - pull by connection: {}", con.info());
                return { key, con.pull() 
            }; }
        }
        return { key, value };
        }
    ));
}

Value OperationBase::execute() {
        if (isNull()) { return Value::error("OperationBase::execute() failed. Caller Operation is null."); }
        Value&& v = this->invoke();

    for (auto& c : outputConnectionList_) {
            c.putToArgumentViaConnection(v);
        }
        return v;

}


Value OperationBase::invoke() {
    logger::trace("OperationBase({})::invoke() called", info());
    if (isNull()) { return Value::error("OperationBase::invoke() failed. Caller Operation is null."); }
    try {
        auto a = collectValues();
        logger::trace("- argument value is {}", a);
        auto v = call(a);
        logger::trace(" - return value is {}", v);
        return std::move(v);
    } catch (const std::exception& ex) {
        return Value::error(logger::error("OperationBase({})::invoke() failed. Exception occurred {}", info(), std::string(ex.what())));
    }
}
/*
Value OperationBase::invoke() {
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    return call(collectValues());
}
*/

Value OperationBase::push(const Value& ci, Value&& value) {
    if (isNull()) { return Value::error(logger::error("{} failed. Caller Operation is null.", __func__)); }
    for (auto& c : inputConnectionListDictionary_[ci.at("input").at("target").at("name").stringValue()]) {
        if (c.info().at("name") == ci.at("name")) {
            return bufferMap_[ci.at("input").at("target").at("name").stringValue()]->push(std::move(value));
        }
    }
    return Value::error(logger::error("OperationBase::push(Value) can not found connection ({})", str(ci)));
}

Value OperationBase::putToArgument(const std::string& argName, const Value& value) {
    if (isNull()) { return Value::error(logger::error("OperationBase({})::{} failed. Caller Operation is null.", info().at("fullName"), __func__)); }
    if (bufferMap_.count(argName) > 0) {
        {
            std::lock_guard<std::mutex> lock(argument_mutex_);
            bufferMap_[argName]->push(value);
            argument_updated_ = true;
        }
        return value;
    }
    return Value::error(logger::error("OperationBase::putToArgument({}) failed.", argName));
}

Value OperationBase::putToArgumentViaConnection(const std::string& argName, const std::string& conName, const Value& value) {
    logger::trace("OperationBase({})::putToArgumentViaConnection({}, {}, {})", getFullName(), argName, conName, value);
    if (isNull()) {
         return Value::error(logger::error("OperationBase::{} failed. Caller Operation is null.", __func__)); 
    }
    if (value.isError()) {
        logger::error("OperationBase::{} failed. Argument Value is Error. ({})", __func__, str(value));
        return value;
    }
    auto connection = this->getInputConnection(argName, conName);
    if (connection.isNull()) {
        logger::error("OperationBase::putToArgumentViaConnection() failed. Connection is null");
    }

    if (bufferMap_.count(argName) > 0) {
        {
            std::lock_guard<std::mutex> lock(argument_mutex_);
            logger::trace(" - pushing value to buffer[{}]", argName);
            bufferMap_[argName]->push(value);
            argument_updated_ = true;
        }
        logger::trace(" - connection type is {}", connection.info());
        if (connection.isEvent()) {
            logger::trace("In OperationBase({})::putToArgumentViaConnection({}) executing myself", info().at("fullName"), conName);
            auto v = execute();
            logger::trace("In OperationBase::putToArgumentViaConnection(): execution result: {}", v);
        }
        return value;
    }
    return Value::error(logger::error("OperationBaseBase::putToArgumentViaConnection({}) failed.", argName));
}

Value OperationBase::removeProviderConnection(const ConnectionInfo& ci) {
    logger::trace("OperationBase::removeProviderConnection({})", str(ci));
    if (isNull()) return Value::error(logger::error("Operation::removeProviderConnection failed. Operation is null"));
    //auto olist = getOutputConnectionList();
    for(auto it = this->outputConnectionList_.begin();it != outputConnectionList_.end();) {
    if ((*it).info().at("name") == ci.at("name")) {
        it = outputConnectionList_.erase(it);
    } else { ++ it; }
    }
    return ci;
}

Value OperationBase::removeConsumerConnection(const std::string& targetArgName, const std::string& conName) {
    logger::trace("OperationBase::removeConsumerConnection({}, {})", targetArgName, conName);
    //if (isNull()) return Value::error(logger::error("Operation::removeConsumerConnection failed. Operation is null"));
    //const auto argName = ci.at("input").at("target").at("name").stringValue();
    //const auto conName = ci.at("name").stringValue();

    for (auto it = inputConnectionListDictionary_[targetArgName].begin(); it != inputConnectionListDictionary_[targetArgName].end();) {
        if ((*it).info().at("name").stringValue() == conName) {
            auto info = it->info();
            it = inputConnectionListDictionary_[targetArgName].erase(it);
            return info;
        } else { ++it; }
    }
    return Value::error(logger::warn("OperationBase::removeConsumerConnection failed. Connection not found."));
}


Value Operation::call(const Value& value) {
    logger::trace("Operation({})::call({})", info().at("fullName"), value);
    std::lock_guard<std::mutex> lock(mutex_);
    bool flag = false;
    argument_mutex_.lock();
    flag = argument_updated_;
    argument_mutex_.unlock();
    if (flag) {
        outputBuffer_.push(std::move(this->function_(value)));
        argument_mutex_.lock();
        argument_updated_ = false;
        argument_mutex_.unlock();
    } else {
        std::cout << "memorized output" << std::endl;
    }
    return getOutput();
}