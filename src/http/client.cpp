#include "nerikiri/http/client.h"

#include <httplib.h>

using namespace nerikiri;

http::Response http::get(std::string&& host, const uint16_t port, std::string&& pattern) {
  httplib::Client client(host.c_str(), port);
  auto result = client.Get(pattern.c_str());
  if (result) {
    return http::Response(result->status, std::move(result->body));
  }
  return http::Response(); // No Connection
}

http::Response http::put(std::string&& host, uint16_t port, std::string&& pattern, std::string&& body, std::string&& content_type) {
  httplib::Client client(host.c_str(), port);
  auto result = client.Put(pattern.c_str(), body.c_str(), content_type.c_str());
  if (result) {
    return http::Response(result->status, std::move(result->body));
  }
  return http::Response();
}
/*
Response get(const std::string& url, const Request& request) {

  httplib::Client cli("localhost", 1234);

  auto res = cli.Get("/hi");
  if (res && res->status == 200) {
    std::cout << res->body << std::endl;
  }

  return Response(); 
}


Response put(const std::string& url, const Request& request) {
  return Response(); 
}


Response post(const std::string& url, const Request& request) {
  return Response(); 
}
*/
