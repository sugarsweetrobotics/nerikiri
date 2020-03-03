




#include "nerikiri/nerikiri.h"
#include "nerikiri/logger.h"
#include "nerikiri/datatype/json.h"
#include "nerikiri/http/httpbroker.h"
#include "webi/http_server.h"
#include "nerikiri/process.h"


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
  nerikiri::Process_ptr process_;
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

  virtual void setProcess(Process_ptr process) override {
    process_ = process;
  }

  bool run() override {
    std::unique_lock<std::mutex> lock(mutex_);
    
    logger::trace("HTTPBroker::run()");
    server_->response("/process/info", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return webi::Response(200, nerikiri::json::toJSONString(process_->info()), "application/json");
    });
    server_->response("/process/operations", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      std::cout << "matches:" << req.matches[1] << std::endl;
      return webi::Response(200, nerikiri::json::toJSONString(process_->getOperationInfos()), "application/json");
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

