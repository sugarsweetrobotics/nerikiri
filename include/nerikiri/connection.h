#pragma once

#include <memory>
#include <vector>
#include <map>

#include "nerikiri/value.h"


namespace nerikiri {

    using ConnectionInfo = nerikiri::Value;

    class Connection {
        private:
        ConnectionInfo info_;
    public:
        Connection(const ConnectionInfo& info);
        ~Connection();

        bool isPull() const { return false; }


    public:
        ConnectionInfo info() const { return info_; }
    };

    using Connection_ptr = std::shared_ptr<Connection>;

    using ConnectionList = std::vector<Connection>;

    using ConnectionListDictionary = std::map<std::string, ConnectionList>;

}
