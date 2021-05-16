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

  class HttpServer {
  public:

  public:
    HttpServer() {}

    HttpServer(HttpServer&& s) {}

    virtual ~HttpServer() {}
    
    virtual void baseDirectory(const std::string& path) = 0;
    
    virtual void response(const std::string& path, const std::string& method, const std::string& contentType, const std::string& content) {
      response(path, method, contentType, [content, contentType](const juiz::Request& req) {
	  juiz::Response response(200, content, contentType);
	return response;
      });
    }

    virtual void response(const std::string& path, const std::string& method, const std::string& contentType, std::function<juiz::Response(const juiz::Request&)> callback) = 0;
    
    
    void responseHTML(const std::string& path, const std::string& method, const std::string& content) {
      response(path, method, "text/html", content);
    }

    void getHTML(const std::string& path, const std::string& content) {
      response(path, "GET", "text/html", content);
    }

    /**
     * Run Server on Current Thread. This function blocks until signal is raised..
     */
    virtual void runForever(const int32_t port=8080) = 0;
    
    /**
     * Run Server on Background Thread.
     * @see waitBackground
     * @see terminateBackground
     */
    virtual void runBackground(const int32_t port=8080) = 0;

    /**
     *
     * @param timeout_sec Timeout Interval. Forever if negative.
     * @return Server is ended if true. Timeout if false.
     */
    virtual bool waitBackground(const double timeout_sec=-1) = 0; 

    virtual void terminateBackground() = 0;
  };
  
  using HttpServer_ptr = std::shared_ptr<HttpServer>;

  juiz::HttpServer_ptr server();
};
