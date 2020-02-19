#pragma once

#include <string>
#include <vector>


namespace nerikiri {
  namespace http {

    struct Header {
      std::string key;
      std::string value;
    };

    struct Request {
      std::string version;
      std::string method;
      std::string target;
      std::string path;
      std::vector<Header> headers;
      std::string body;
    Request(const std::string& m, const std::string& b) : method(m), body(b) {} 
    Request(std::string&& m, std::string&& b) : method(m), body(b) {} 
    };

    struct Response {
      std::string version;
      int32_t status;
      std::vector<Header> headers;
      std::string body;
    Response(): version("1.0"), status(0) {}
    Response(int32_t s): version("1.0"), status(s), body("") {}
    Response(int32_t s,  std::string&& body) : version("1.0"), status(s), body(body) {}
    Response(Response&& r) : version(r.version), status(r.status), body(r.body) {}
	
    };

    //Response get(const std::string& url, const Request& request);
    Response get(std::string&& host, uint16_t port, std::string&& pattern);
    Response put(std::string&& host, uint16_t port, std::string&& pattern, std::string&& body, std::string&& content_type);
    Response get(const std::string& url, const Request& request);
    Response put(const std::string& url, const Request& request);
    Response post(const std::string& url, const Request& request);
};
};
