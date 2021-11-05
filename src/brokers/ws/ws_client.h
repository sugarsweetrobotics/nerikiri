#pragma once

#include <iostream>

#include <cstdint>
#include <string>
#include <functional>
#include <memory>

#include "ws.h"

namespace juiz {
  namespace ws {
  class WebSocketClient {
  public:
    
  public:
    WebSocketClient() {}

    WebSocketClient(WebSocketClient&& s) {}

    virtual ~WebSocketClient() {}

  public:
    
    virtual void setTimeout(double seconds) = 0;
    virtual Response request(const std::string& url, const std::string& method) = 0;
    virtual Response request(const std::string& url, const std::string& method, const Request& req) = 0;
  };
  
  using WebSocketClient_ptr = std::shared_ptr<WebSocketClient>;

  WebSocketClient_ptr client(const std::string& addr, const int port);
}
}
