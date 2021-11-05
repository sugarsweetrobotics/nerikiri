#include <iostream>

#include "ws_server.h"

#include <ixwebsocket/IXWebSocketServer.h>

#include "ws_converter_ixwebsocket.h"

using namespace juiz;
using namespace juiz::ws;

class WSServerImpl : public WSServer {
private:
  std::unique_ptr<ix::WebSocketServer> server_;
  std::unique_ptr<std::thread> thread_;
public:
  WSServerImpl();
  virtual ~WSServerImpl();

  WSServerImpl(WSServerImpl&& server);

  void baseDirectory(const std::string& path) override;
  
  void response(const std::string& path, const std::string& method, const std::string& contentType, std::function<juiz::ws::Response(const juiz::ws::Request&)> callback) override;

  void runForever(const int32_t port=8080) override;
    
  void runBackground(const int32_t port=8080) override;

  bool waitBackground(const double timeout_sec=-1) override;

  void terminateBackground() override;
};


WSServerImpl::WSServerImpl() {
}

WSServerImpl::WSServerImpl(WSServerImpl&& server) {
}

WSServerImpl::~WSServerImpl() {
  terminateBackground();
  if (thread_) {
    thread_->join();
  }
}

void WSServerImpl::baseDirectory(const std::string& path) {
//  server_.set_base_dir(path.c_str());
  //server_.set_mount_point(nullptr, path.c_str());
}

void WSServerImpl::response(const std::string& path, const std::string& method, const std::string& contentType, std::function<juiz::ws::Response(const juiz::ws::Request&)> callback) {
  
  /*
  if (method == "GET") {
    server_.Get(path.c_str(), [callback, contentType](const httplib::Request& req, httplib::Response& res) {
	apply(res, callback(convert(req)));
      });

  } else if (method == "PUT") {
    server_.Put(path.c_str(), [callback, contentType](const httplib::Request& req, httplib::Response& res) {
	apply(res, callback(convert(req)));
      });
  }
  else if (method == "POST") {
    server_.Post(path.c_str(), [callback, contentType](const httplib::Request& req, httplib::Response& res) {
	apply(res, callback(convert(req)));
      });
  }
  else if (method == "DELETE") {
    server_.Delete(path.c_str(), [callback, contentType](const httplib::Request& req, httplib::Response& res) {
	apply(res, callback(convert(req)));
      });
  }
  */

}

void WSServerImpl::runForever(const int32_t port /*=8080*/) {
  server_ = std::make_unique<ix::WebSocketServer>(port);
  server_->listen();
  server_->start();
  server_->wait();
}

void WSServerImpl::runBackground(const int32_t port /*=8080*/) {
  thread_ = std::make_unique<std::thread>([port, this] {
	  this->runForever(port);
  });
}

bool WSServerImpl::waitBackground(const double timeout_sec) {
  auto t = std::chrono::system_clock::now();
  for(;;) {
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    //if (!server_.is_running()) {
    //  return true; // Server is ended
    //}
    if (timeout_sec < 0) continue;

    auto d = std::chrono::system_clock::now() - t; // time_point
    int to = (int)(timeout_sec*1000);
    if (to > 0 && d > std::chrono::milliseconds(to)) {
      return false; // Timeout.
    }
  }
}

void WSServerImpl::terminateBackground() {
  server_->stop();
}
/*
HttpServer_ptr WebiImpl::createHttpServerImpl() {
  return std::make_shared<HttpServerImpl>();
}*/

juiz::ws::WSServer_ptr juiz::ws::server() {
  return juiz::ws::WSServer_ptr(new WSServerImpl());
}
