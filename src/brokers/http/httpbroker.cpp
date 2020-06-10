#include <cstdlib>
#include "webi/http_client.h"
#include "webi/http_server.h"

#include "nerikiri/nerikiri.h"
#include "nerikiri/logger.h"
#include "nerikiri/json.h"
#include "./HTTPBroker.h"
#include "nerikiri/process.h"
#include "nerikiri/objectmapper.h"

using namespace nerikiri;
using namespace nerikiri::http;
using namespace nerikiri::logger;


extern "C" {
    NK_OPERATION  void* createHTTPBroker();
};



class HTTPBrokerImpl : public HTTPBroker {
private:
  webi::HttpServer_ptr server_;
  std::condition_variable cond_;
  std::mutex mutex_;
  std::string address_;
  int32_t port_;
public:

  HTTPBrokerImpl(const std::string& address, const int32_t port, const std::string& base_dir="."): HTTPBroker(), server_(webi::server()), address_(address), port_(port) {
    logger::trace("HTTPBroker::HTTPBroker()");
    server_->baseDirectory(base_dir);
  }

  virtual ~HTTPBrokerImpl() {
    logger::trace("HTTPBroker::~HTTPBroker()");
  }

  webi::Response response(std::function<Value(void)> func) {
    try {
      return webi::Response(200, nerikiri::json::toJSONString(func()), "application/json");
    } catch (nerikiri::json::JSONParseError& e) {
      logger::error("JSON Parse Error in HTTPBroker::response(): \"{}\"", e.what());
      return webi::Response(400, "Bad Request", "text/html");
    } catch (nerikiri::json::JSONConstructError& e) {
      logger::error("JSON Construct Error in HTTPBroker::response(): \"{}\"", e.what());
      return webi::Response(400, e.what(), "text/html");
    } catch (nerikiri::ValueTypeError &e) {
      logger::error("ValueTypeError in HTTPBroker::response(): \"{}\"", e.what());
      return webi::Response(400, e.what(), "text/html");
    } catch (std::exception &e) {
      logger::error("Exception in HTTPBroker::response(): \"{}\"", e.what());
      return webi::Response(400, e.what(), "text/html");
    }
  }

  bool run(Process* process) override {
    std::unique_lock<std::mutex> lock(mutex_);

    //server_->baseDirectory(".");
    
    logger::trace("HTTPBroker::run()");

    server_->response("/broker/info/", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){
        auto _info = info();
        for(auto h : req.headers) {
          if (h.first == "Host") {
            std::string host = h.second;
            int64_t port = 80;
            std::string addr = h.second;
            size_t pos = host.find(":");
            if (pos != std::string::npos) {
              auto port_str = host.substr(pos+1);
                port = atoi(port_str.c_str());
                addr = host.substr(0, pos);
            }

            _info["host"] = Value(addr);
            _info["port"] = Value(port);
          }
        }
        return _info;
        });
    });

    server_->response("/process/.*", "GET", "text/html", [this, process](const webi::Request& req) -> webi::Response {
      logger::trace("HTTPBroker::Response(url='{}')", req.matches[0]);
      return response([process, &req](){
        //return process->readResource(req.matches[0]);
        return process->coreBroker()->readResource(req.matches[0]);
      });
    });

    server_->response("/.*", "POST", "text/html", [this, process](const webi::Request& req) -> webi::Response {
      logger::trace("HTTPBroker::Response(url='{}')", req.matches[0]);
      return response([this, process, &req](){
        //return process->createResource(req.matches[0], nerikiri::json::toValue(req.body));
        return process->coreBroker()->createResource(req.matches[0], nerikiri::json::toValue(req.body));
      });
    });

    server_->response("/.*", "DELETE", "text/html", [this, process](const webi::Request& req) -> webi::Response {
      return response([this, process, &req](){
        //return process->deleteResource(req.matches[0]);
        return process->coreBroker()->deleteResource(req.matches[0]);
      });
    });

    server_->response("/.*", "PUT", "text/html", [this, &process](const webi::Request& req) -> webi::Response {
      return response([this, &process, &req](){
        //return process->updateResource(req.matches[0], nerikiri::json::toValue(req.body)); 
        return process->coreBroker()->updateResource(req.matches[0], nerikiri::json::toValue(req.body)); 
      });
    });

    Broker::run(process);
    server_->runBackground(port_);
    cond_.wait(lock);
    
    server_->terminateBackground();
    return true;
  }

  void shutdown(Process* proc) override {
    Broker::shutdown(proc);
    cond_.notify_all();
  }

};

std::shared_ptr<nerikiri::Broker> nerikiri::http::broker(const std::string& address, const int32_t port) {
  return std::shared_ptr<nerikiri::Broker>(new HTTPBrokerImpl(address, port));
}


std::shared_ptr<nerikiri::Broker> nerikiri::http::HTTPBrokerFactory::create(const Value& value) {
  auto address = value.at("host").stringValue();
  auto port = value.at("port").intValue();
  std::string base_dir = ".";
  if (value.hasKey("baseDir")) {
    base_dir = value.at("baseDir").stringValue();
  }
  return std::shared_ptr<nerikiri::Broker> (new HTTPBrokerImpl(address, port, base_dir));
}

std::shared_ptr<nerikiri::BrokerAPI> nerikiri::http::HTTPBrokerFactory::createProxy(const Value& value) {
  auto address = value.at("host").stringValue();
  auto port = value.at("port").intValue();
  return std::shared_ptr<nerikiri::BrokerAPI> (new HTTPBrokerProxyImpl(address, port));
}

void* createHTTPBroker() {
    return new HTTPBrokerFactory();
}
