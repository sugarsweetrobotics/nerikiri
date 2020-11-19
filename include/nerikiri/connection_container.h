#pragma once


#include <map>
#include <functional>
#include <thread>
#include <mutex>

#include "nerikiri/nerikiri.h"
#include "nerikiri/object.h"
#include "nerikiri/operation_api.h"

#include "nerikiri/connection.h"
#include "nerikiri/functional.h"

#include "nerikiri/newest_value_buffer.h"

#include "nerikiri/operation_inlet_base.h"

namespace nerikiri {

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
          return Value::error(logger::error("ConnectionContainer::addConnection({}) failed. Outlet already has the same name connection.", con->fullName()));
        }
      }
      connections_.push_back(con);
      return con->info();
    }

    Value removeConnection(const std::string& _fullName) {
      for(auto it = connections_.begin(); it != connections_.end();++it) {
        if ((*it)->fullName() == _fullName) {
          connections_.erase(it);
          return (*it)->info();
        }
      }
      return Value::error(logger::warn("ConnectionContainer::removeConnection({}) failed. Connection not found.", _fullName));
    }
  };

}