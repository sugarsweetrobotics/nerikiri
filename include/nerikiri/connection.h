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
        Broker_ptr broker_;
    public:
        Connection(const ConnectionInfo& info, Broker_ptr broker);
        ~Connection();

        bool isPull() const { return false; }


    public:
        ConnectionInfo info() const { return info_; }
    };

    using Connection_ptr = std::shared_ptr<Connection>;

    using ConnectionList = std::vector<Connection>;

    using ConnectionListDictionary = std::map<std::string, ConnectionList>;

}
