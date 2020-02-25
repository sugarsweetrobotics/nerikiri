#pragma once

#include <string>
#include <stdint.h>
#include <functional>
#include <memory>
#include "client.h"
#include "nerikiri/logger.h"

namespace nerikiri::http {

    struct HttpServer {
      std::string address;
      int32_t port;
      bool failed;
      
      HttpServer(): failed(false) {}
      HttpServer(const std::string& addr, const int32_t p): address(addr), port(p), failed(false) {}
      HttpServer(HttpServer&& s) : address(s.address), port(s.port), failed(false) {}
      HttpServer(const HttpServer& s) : address(s.address), port(s.port), failed(false) {}
      virtual ~HttpServer() {}
    };

  using HttpServer_ptr = std::shared_ptr<HttpServer>;
  HttpServer_ptr server(const std::string& address, const int32_t port);
  
  typedef std::function<Response(Request&&)> Callback;
  
  std::function<HttpServer_ptr(HttpServer_ptr&&)> serve(const std::string& endpoint, const std::string& method, Callback cb);
  inline HttpServer_ptr serve(const std::string& endpoint, const std::string& method, Callback cb, HttpServer_ptr&& server) {
    return serve(endpoint, method, cb)(std::move(server));
  }

    std::function<HttpServer_ptr(HttpServer_ptr&&)> listen(double timeout);
    inline HttpServer_ptr listen(double timeout, HttpServer_ptr&& server) {
      return listen(timeout)(std::move(server));
    }

    std::function<HttpServer_ptr(HttpServer_ptr&&)>  stop();

};
