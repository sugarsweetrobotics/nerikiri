#pragma once

#include <vector>
#include <map>
#include <thread>
#include <future>
#include "nerikiri/functional.h"
#include "nerikiri/connection.h"

namespace nerikiri {

  class ConnectionDictionary {
  private:
    std::map<std::string, Connection_ptr> dictionary_;

  public:
    void add(Connection_ptr&& connection);

    void foreach(std::function<void(Connection_ptr&)> func);
    
    template<typename T>
    std::vector<T> map(std::function<T(Connection_ptr&)> func) {
      logger::trace("ConnectionDictionary::map()");
      std::vector<T> ts;
      for(auto& [name, connection] : dictionary_) {
        logger::trace(" - for connection {}", str(connection->info()));
        ts.emplace_back(func(connection));
      }
      logger::trace("ConnectionDictionary::map ends");
      return ts;
    }
  };

};
