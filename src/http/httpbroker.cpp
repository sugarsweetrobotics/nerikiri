




#include "nerikiri/nerikiri.h"
#include "nerikiri/logger.h"
#include "nerikiri/http/httpbroker.h"



using namespace nerikiri;
using namespace nerikiri::http;
using namespace nerikiri::logger;


HTTPBroker::HTTPBroker(const std::string& address, const int32_t port): Broker({"HTTPBroker"}), server_(server(address, port)) {
  logger::trace("HTTPBroker::HTTPBroker({})", str(info()));
}

HTTPBroker::~HTTPBroker() {
  logger::trace("HTTPBroker::~HTTPBroker()");
}

auto get_cb = [](HTTPBroker* broker) {
		return [](http::Request&& r) {
			 return http::Response(200, "Hello");
		       };
	      };


bool HTTPBroker::run() {
  std::unique_lock<std::mutex> lock(mutex_);
  
  logger::trace("HTTPBroker::run()");
  auto svc = compose<http::Server_ptr>(http::serve("/", "GET", get_cb(this)),
		      http::listen(1.0));
  server_ = svc(server_);
  cond_.wait(lock);
  return true;
}

void HTTPBroker::shutdown() {
  cond_.notify_all();
}
