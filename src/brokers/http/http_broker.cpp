#include <cstdlib>
#include <string>
#include <condition_variable>
#include <fstream>
#include <map>

#include <nerikiri/crud_broker.h>
#include <nerikiri/broker_factory_api.h>
#include "http_broker_factory.h"
#include "http_client.h"
#include "http_client.h"
#include "http_server.h"

using namespace nerikiri;

extern "C" {
    NK_OPERATION  void* createHTTPBroker();
};

/**
 * 
 * 
 */
class HTTPBroker : public CRUDBrokerBase {
private:
  nerikiri::HttpServer_ptr server_;
  std::string address_;
  int32_t port_;
  std::string baseDirectory_;
  std::condition_variable cond_;
  std::mutex mutex_;
  std::map<std::string, std::string> route_;
  std::string endpoint_;
public:

  HTTPBroker(const std::string& address, const int32_t port, const std::string& base_dir=".", const Value& value=Value::error("null")): 
    CRUDBrokerBase("HTTPBroker", "httpBroker"),
    server_(nerikiri::server()), address_(address), port_(port), baseDirectory_(base_dir), endpoint_("httpBroker")
  {
    logger::trace("HTTPBroker::HTTPBroker()");

    if (!value.isError()) {
      value.const_object_for_each([this](auto k, auto v) {
        route_[k] = v.stringValue();
      });
    }
  }

  virtual ~HTTPBroker() {
    logger::trace("HTTPBroker::~HTTPBroker()");
  }

  nerikiri::Response toResponse(const Value& value) const {
    try {
        return nerikiri::Response(200, nerikiri::json::toJSONString(value), "application/json");
    } catch (nerikiri::json::JSONParseError& e) {
        logger::error("JSON Parse Error in HTTPBroker::response(): \"{}\"", e.what());
        return nerikiri::Response(400, "Bad Request", "text/html");
    } catch (nerikiri::json::JSONConstructError& e) {
        logger::error("JSON Construct Error in HTTPBroker::response(): \"{}\"", e.what());
        return nerikiri::Response(400, e.what(), "text/html");
    } catch (nerikiri::ValueTypeError &e) {
        logger::error("ValueTypeError in HTTPBroker::response(): \"{}\"", e.what());
        return nerikiri::Response(400, e.what(), "text/html");
    } catch (std::exception &e) {
        logger::error("Exception in HTTPBroker::response(): \"{}\"", e.what());
        return nerikiri::Response(400, e.what(), "text/html");
    }
  }

  nerikiri::Value toValue(const std::string& body) {
    return nerikiri::json::toValue(body);
  }
  
  virtual Value fullInfo() const override {
    auto i = info();
    i["port"] = port_;
    i["address"] = address_;
    i["baseDirectory"] = baseDirectory_;
    return i;
  }

  virtual bool run(Process* process) override {
    std::unique_lock<std::mutex> lock(mutex_);
    logger::trace("HTTPBroker::run()");

    server_->baseDirectory(baseDirectory_);


    const std::string endpoint = "/" + endpoint_ + "/(.*)";
    /*
    server_->response("/broker/info/", "GET", "text/html", [this](const nerikiri::Request& req) -> nerikiri::Response {
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
    */

    server_->response(endpoint, "GET", "text/html", [this, process](const nerikiri::Request& req) -> nerikiri::Response {
      logger::trace("HTTPBroker::Response(url='{}')", req.matches[1]);
      const std::string path = req.matches[1];
      return toResponse(onRead(process, req.matches[1]));
    });

    server_->response(endpoint, "POST", "text/html", [this, process](const nerikiri::Request& req) -> nerikiri::Response {
      logger::trace("HTTPBroker::Response(url='{}')", req.matches[1]);
      return toResponse(onCreate(process, req.matches[1], toValue(req.body)));
    });

    server_->response(endpoint, "DELETE", "text/html", [this, process](const nerikiri::Request& req) -> nerikiri::Response {
      logger::trace("HTTPBroker::Response(url='{}')", req.matches[1]);
      return toResponse(onDelete(process, req.matches[1]));
    });

    server_->response(endpoint, "PUT", "text/html", [this, &process](const nerikiri::Request& req) -> nerikiri::Response {
      logger::trace("HTTPBroker::Response(url='{}')", req.matches[1]);
      return toResponse(onUpdate(process, req.matches[1], toValue(req.body)));
    });


    for(auto [k, v] : route_) {
      const std::string path = v;
      server_->response(k, "GET", "text/html", [this, path, &process](const nerikiri::Request& req) -> nerikiri::Response {
        const std::string relpath = req.matches[1];
        return nerikiri::Response(path + relpath);
      });
    }

    CRUDBrokerBase::run(process);
    server_->runBackground(port_);
    cond_.wait(lock);
    
    server_->terminateBackground();
    return true;
  }

  void shutdown(Process* proc) override {
    CRUDBrokerBase::shutdown(proc);
    cond_.notify_all();
  }

};


std::shared_ptr<BrokerAPI> HTTPBrokerFactory::create(const Value& value) {
  if (!value.hasKey("host")) {
    logger::error("HTTPBrokerFactory::create({}) failed. There is no 'host' value.", value);
    return std::make_shared<NullBroker>();
  }
  if (!value.hasKey("port")) {
    logger::error("HTTPBrokerFactory::create({}) failed. There is no 'port' value.", value);
    return std::make_shared<NullBroker>();
  }

  std::string base_dir = ".";
  if (value.hasKey("baseDir")) {
    base_dir = value.at("baseDir").stringValue();
  }
  return std::make_shared<HTTPBroker>(value.at("host").stringValue(), value.at("port").intValue(), Value::string(value.at("baseDir"), "."), value.at("route"));
}


void* createHTTPBroker() {
    return new HTTPBrokerFactory("httpBroker");
}
