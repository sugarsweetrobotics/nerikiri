#pragma once

#include <iostream>

#include <cstdint>
#include <string>
#include <functional>
#include <memory>

//#include "webi_common.h"

//#include "webi/html.h"
#include "http.h"

namespace juiz {

  class HttpClient {
  public:
    
  public:
    HttpClient() {}

    HttpClient(HttpClient&& s) {}

    virtual ~HttpClient() {}

  public:
    
    virtual void setTimeout(double seconds) = 0;
    virtual Response request(const std::string& url, const std::string& method) = 0;
    virtual Response request(const std::string& url, const std::string& method, const Request& req) = 0;
  };
  
  using HttpClient_ptr = std::shared_ptr<HttpClient>;

  HttpClient_ptr client(const std::string& addr, const int port);
};
