
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <regex>


namespace juiz {
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
    std::map<std::string, std::string> params;
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
    std::ifstream file_;
    bool is_file_;
    Response() : version("1.0"), status(0), is_file_(false) {}
    Response(int32_t s) : version("1.0"), status(s), body(""), is_file_(false) {}
    Response(int32_t s, std::string &&body, std::string&&  contentType) : version("1.0"), status(s), body(body), contentType(contentType), is_file_(false) {}
    Response(const std::string& filepath) : version("1.0"), file_(filepath, std::ios_base::binary) {
      if (file_.is_open()) {
        status = 200;
        contentType = "text/html";
        is_file_ = false;
        file_.seekg(0, std::ios_base::end);
        auto size = file_.tellg();
        file_.seekg(0);
        body.resize(size);
        file_.read(&body[0], size);
      } else {
        status = 404;
        contentType = "text/html";
        is_file_ = false;
      }      
    }
    Response(int32_t s, const std::string &body, const std::string& contentType) : version("1.0"), status(s), body(body), contentType(contentType), is_file_(false) {}
    Response(Response &&r) : version(r.version), status(r.status), body(r.body), is_file_(false) {}
    Response& operator=(const Response&& r) {
      version = r.version;
      status = r.status;
      headers = r.headers;
      body = std::move(r.body);
      return *this;}
  };
  
}