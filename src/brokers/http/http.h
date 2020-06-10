
#pragma once

#include <string>
#include <vector>
#include <regex>


namespace nerikiri {
  using Header = std::pair<std::string, std::string>;

  struct Request {
  public:
    std::string version;
    std::string method;
    std::string target;
    std::string path;
    std::vector<Header> headers;
    std::string body;
    std::string contentType;
    std::smatch matches;
    void* _privateData;
    Request(const std::string &m, const std::string &b, const std::string &ct) : method(m), body(b), _privateData(nullptr), contentType(ct) {}
    Request(std::string &&m, std::string &&b, std::string& ct) : method(m), body(b), _privateData(nullptr), contentType(ct) {}
    Request(const std::string &m, const std::string &b, const std::string& contentType, const std::smatch &ms, void* privateData=nullptr) :
        method(m), body(b), matches(ms), _privateData(privateData), contentType(contentType) {}

    Request(std::string &&m, std::string &&b, std::string&& ct, std::smatch&& ms) : method(m), body(b), matches(ms), _privateData(nullptr), contentType(ct) {}

    Request(std::string &&m, std::string &&b, std::vector<Header>&& headers, std::smatch&& ms) : method(m), body(b), matches(ms), _privateData(nullptr), headers(headers) {
        for(auto& h : headers) {
        if (h.first == "Content-Type") contentType = h.second;
        }
    }
    Request(const std::string &m, const std::string &b, const std::vector<Header>& headers, const std::smatch& ms) : method(m), body(b), matches(ms), _privateData(nullptr), headers(headers) {
        for(auto& h : headers) {
        if (h.first == "Content-Type") contentType = h.second;
        }
    }

    Request(std::string &&m, std::string &&b, std::vector<Header>&& headers) : method(m), body(b), _privateData(nullptr), headers(headers) {
        for(auto& h : headers) {
        if (h.first == "Content-Type") contentType = h.second;
        }
    }
    Request(const std::string &&m, const std::string &b, const std::vector<Header>& headers) : method(m), body(b), _privateData(nullptr), headers(headers) {
        for(auto& h : headers) {
        if (h.first == "Content-Type") contentType = h.second;
        }
    }
  };


  struct Response {
  public:
    std::string version;
    int32_t status;
    std::vector<Header> headers;
    std::string body;
    std::string contentType;
    Response() : version("1.0"), status(0) {}
    Response(int32_t s) : version("1.0"), status(s), body("") {}
    Response(int32_t s, std::string &&body, std::string&&  contentType) : version("1.0"), status(s), body(body), contentType(contentType) {}
    Response(int32_t s, const std::string &body, const std::string& contentType) : version("1.0"), status(s), body(body), contentType(contentType) {}
    Response(Response &&r) : version(r.version), status(r.status), body(r.body) {}
    Response& operator=(const Response&& r) {
      version = r.version;
      status = r.status;
      headers = r.headers;
      body = std::move(r.body);
      return *this;}
  };
  
}