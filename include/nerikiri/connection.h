#pragma once

#include <memory>
#include <vector>
#include <map>

#include "nerikiri/value.h"
#include "nerikiri/broker.h"


namespace nerikiri {

    class InvalidBrokerException : public std::exception {};

    using ConnectionInfo = nerikiri::Value;

    class Connection {
    private:
        ConnectionInfo info_;
        Broker_ptr providerBroker_;
        Broker_ptr consumerBroker_;
        std::function<Value()> pull_func_;
        std::function<Value(Value&& value)> push_func_;
        bool is_null_;
    public:
        Connection();
        Connection(const ConnectionInfo& info, Broker_ptr providerBroker, Broker_ptr consumerBroker);
        ~Connection();

        bool isPull() const { return true; }

        Value pull() { return this->pull_func_(); }

        bool isPush() const { return true; }

        Value push(Value&& value) { return this->push_func_(std::move(value)); }

        bool isNull() const { return is_null_; }

        static Connection null;

    public:
        ConnectionInfo info() const { return info_; }
    };

    using Connection_ptr = std::shared_ptr<Connection>;

    using ConnectionList = std::vector<Connection>;

    using ConnectionListDictionary = std::map<std::string, ConnectionList>;


    inline Connection providerConnection(const ConnectionInfo& info, Broker_ptr consumerBroker) {
        return Connection(info, nullptr, consumerBroker);
    }

    inline Connection consumerConnection(const ConnectionInfo& info, Broker_ptr providerBroker) {
        return Connection(info, providerBroker, nullptr);
    }

}
