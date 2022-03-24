#pragma once


#include <map>
#include <functional>
#include <thread>
#include <mutex>

#include <juiz/juiz.h>
#include <juiz/object.h>
#include <juiz/operation_api.h>

#include <juiz/connection.h>
#include <juiz/utils/functional.h>

#include "newest_value_buffer.h"

#include <juiz/operation_api.h>

namespace juiz {

  class ConnectionContainer {
  private:
    std::vector<std::shared_ptr<ConnectionAPI>> connections_;
  public:
    ~ConnectionContainer() {}

  public:
    std::vector<std::shared_ptr<ConnectionAPI>> connections() const { return connections_; }

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& con) {
      for(auto c : connections()) {
        if (c->fullName() == con->fullName()) {
          return Value::error(logger::warn("ConnectionContainer::addConnection({}) failed. ConnectionContainer already has the same name connection.", con->fullName()));
        }
      }
      connections_.push_back(con);
      return con->info();
    }

    Value removeConnection(const std::string& _fullName) {
      for(auto it = connections_.begin(); it != connections_.end();++it) {
        if ((*it)->fullName() == _fullName) {
          auto i = (*it)->info();
          connections_.erase(it);
          return i;
        }
      }
      return Value::error(logger::warn("ConnectionContainer::removeConnection({}) failed. Connection not found.", _fullName));
    }

    void clear() {
      connections_.clear();
    }
  };

}