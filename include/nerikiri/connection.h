#pragma once

#include <memory>
#include <vector>
#include <map>

#include "nerikiri/value.h"
#include "nerikiri/brokers/brokerapi.h"


namespace nerikiri {

    class InvalidBrokerException : public std::exception {};

    using ConnectionInfo = nerikiri::Value;

    class Connection {
    private:
        ConnectionInfo info_;
        std::shared_ptr<BrokerAPI> providerBroker_;
        std::shared_ptr<BrokerAPI> consumerBroker_;
        std::function<Value()> pull_func_;
        std::function<Value(const Value& value)> push_func_;
        bool is_null_;
        bool is_event_;
    public:
        Connection();
        Connection(const ConnectionInfo& info, std::shared_ptr<BrokerAPI> providerBroker, std::shared_ptr<BrokerAPI> consumerBroker);
        ~Connection() {}

        Connection(const Connection& c) : info_(c.info_), 
        providerBroker_(c.providerBroker_), consumerBroker_(c.consumerBroker_), 
        pull_func_(c.pull_func_), push_func_(c.push_func_),
        is_null_(c.is_null_), is_event_(c.is_event_) {}

        bool isPull() const { return true; }

        Value pull() { return this->pull_func_(); }

        bool isPush() const { return true; }

        Value putToArgumentViaConnection(const Value& value) { return this->push_func_(value); }

        bool isNull() const { return is_null_; }

        bool isEvent() const { return is_event_; }

        static Connection null;

    public:
        ConnectionInfo info() const { return info_; }
    };

    using Connection_ptr = std::shared_ptr<Connection>;

    using ConnectionList = std::vector<Connection>;

    using ConnectionListDictionary = std::map<std::string, ConnectionList>;


    inline Connection providerConnection(const ConnectionInfo& info, std::shared_ptr<BrokerAPI> consumerBroker) {
        return Connection(info, nullptr, consumerBroker);
    }

    inline Connection consumerConnection(const ConnectionInfo& info, std::shared_ptr<BrokerAPI> providerBroker) {
        return Connection(info, providerBroker, nullptr);
    }

}
