#pragma once

#include <string>
#include <stdint.h>
#include <functional>
#include <memory>
#include "client.h"
#include "nerikiri/logger.h"

namespace nerikiri::http {

    struct Server {
      std::string address;
      int32_t port;
      bool failed;
      
      Server(): failed(false) {}
      Server(const std::string& addr, const int32_t p): address(addr), port(p), failed(false) {}
      Server(Server&& s) : address(s.address), port(s.port), failed(false) {}
      Server(const Server& s) : address(s.address), port(s.port), failed(false) {}
      virtual ~Server() {}
    };

  using Server_ptr = std::shared_ptr<Server>;
  Server_ptr server(const std::string& address, const int32_t port);
  
  typedef std::function<Response(Request&&)> Callback;
  
  std::function<Server_ptr(Server_ptr&&)> serve(const std::string& endpoint, const std::string& method, Callback cb);
  inline Server_ptr serve(const std::string& endpoint, const std::string& method, Callback cb, Server_ptr&& server) {
    return serve(endpoint, method, cb)(std::move(server));
  }

    std::function<Server_ptr(Server_ptr&&)> listen(double timeout);
    inline Server_ptr listen(double timeout, Server_ptr&& server) {
      return listen(timeout)(std::move(server));
    }

    std::function<Server_ptr(Server_ptr&&)>  stop();

};
