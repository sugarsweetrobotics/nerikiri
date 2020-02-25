




#include "nerikiri/nerikiri.h"
#include "nerikiri/logger.h"
#include "nerikiri/http/httpbroker.h"
#include "webi/http_server.h"


using namespace nerikiri;
using namespace nerikiri::http;
using namespace nerikiri::logger;


auto get_cb = [](HTTPBroker* broker) {
		return [](http::Request&& r) {
			 return http::Response(200, "Hello");
		       };
	      };


class HTTPBrokerImpl : public HTTPBroker {
private:
//HttpServer_ptr server_;
  webi::HttpServer_ptr server_;
  std::condition_variable cond_;
  std::mutex mutex_;
  std::string address_;
  int32_t port_;
public:
  HTTPBrokerImpl(const std::string& address, const int32_t port): HTTPBroker(), server_(webi::server()), address_(address), port_(port) {
    logger::trace("HTTPBroker::HTTPBroker({})", str(info()));
  }
  virtual ~HTTPBrokerImpl() {
    logger::trace("HTTPBroker::~HTTPBroker()");
  }

  bool run() override {
    std::unique_lock<std::mutex> lock(mutex_);
    
    logger::trace("HTTPBroker::run()");
    server_->response("/", "GET", "text/html", [](const webi::Request& req) -> webi::Response {
      webi::Response r(200);
      r.contentType = "text/html";
      r.body = "Hello WebiTypeHTTPBroker";
      return r;
    });
    server_->runBackground(port_);
    cond_.wait(lock);
    
    server_->terminateBackground();
    return true;
  }

  void shutdown() override {
    cond_.notify_all();
  }

};

nerikiri::Broker_ptr nerikiri::http::broker(const std::string& address, const int32_t port) {
  return nerikiri::Broker_ptr(new HTTPBrokerImpl(address, port));
}

