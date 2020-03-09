#pragma once

#include <memory>
#include <vector>
#include <map>

#include "nerikiri/value.h"
#include "nerikiri/broker.h"


namespace nerikiri {

    using ConnectionInfo = nerikiri::Value;

    class Connection {
        private:
        ConnectionInfo info_;
        Broker_ptr providerBroker_;
        Broker_ptr consumerBroker_;
        std::function<Value()> pull_func_;
    public:
        Connection(const ConnectionInfo& info, Broker_ptr providerBroker, Broker_ptr consumerBroker);
        ~Connection();

        bool isPull() const { return true; }

        Value pull() { return this->pull_func_(); }

    public:
        ConnectionInfo info() const { return info_; }
    };

    using Connection_ptr = std::shared_ptr<Connection>;

    using ConnectionList = std::vector<Connection>;

    using ConnectionListDictionary = std::map<std::string, ConnectionList>;

}
