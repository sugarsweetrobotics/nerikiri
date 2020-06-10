#include <math.h>


#include "http_client.h"

#include "cpp-httplib/httplib.h"

#include "http_converter_cpphttplib.h"

using namespace nerikiri;


class HttpClientImpl : public HttpClient {
private:
  httplib::Client client_;
public:
  HttpClientImpl(const std::string& addr, const int port) : client_(addr.c_str(), port) {
  }

  virtual ~HttpClientImpl() {}
public:
  virtual void setTimeout(double sec) {
    double to_sec = floor(sec);
    double m = sec - to_sec;
    double to_usec = floor(m/1000000);      
    client_.set_read_timeout(to_sec, to_usec);
    client_.set_write_timeout(to_sec, to_usec);
  }
public:
  virtual Response request(const std::string& url, const std::string& method) {
    if (method == "GET") {
        auto response = client_.Get(url.c_str());
        return convert(std::move(response));
    } else if (method == "POST") {
        auto response = client_.Post(url.c_str(), "", "text/html");
        return convert(response);
    } else if (method == "PUT") {
        auto response = client_.Put(url.c_str(),  "", "text/html");
        return convert(response);
    } else if (method == "DELETE") {
        auto response = client_.Delete(url.c_str(),  "", "text/html");
        return convert(response);
    }
    return Response(405);
  }

  virtual Response request(const std::string& url, const std::string& method, const Request& req) {
      if (method == "GET") {
          auto response = client_.Get(url.c_str());
          return convert(response);
      } else if (method == "POST") {
          auto response = client_.Post(url.c_str(), req.body.c_str(), req.contentType.c_str());
          return convert(response);
      } else if (method == "PUT") {
          auto response = client_.Put(url.c_str(), req.body.c_str(), req.contentType.c_str());
          return convert(response);
      }
      return Response(405);
  }
  
};



nerikiri::HttpClient_ptr nerikiri::client(const std::string& addr, const int port) {
    return nerikiri::HttpClient_ptr(new HttpClientImpl(addr, port));
}
