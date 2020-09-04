#include <cstdlib>
#include <string>
#include <condition_variable>
#include <fstream>
#include <map>

#include "nerikiri/nerikiri.h"
#include "nerikiri/logger.h"
#include "nerikiri/json.h"

#include "nerikiri/process.h"
#include "nerikiri/objectmapper.h"



#include "nerikiri/broker.h"
#include "nerikiri/abstractbrokerproxy.h"
#include "nerikiri/brokerfactory.h"
#include "nerikiri/nullbrokerproxy.h"

#include "nerikiri/json.h"
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
class HTTPBroker : public Broker {
private:
  nerikiri::HttpServer_ptr server_;
  std::string address_;
  int32_t port_;
  std::string baseDirectory_;
  std::condition_variable cond_;
  std::mutex mutex_;
  std::map<std::string, std::string> route_;
public:

  HTTPBroker(const std::string& address, const int32_t port, const std::string& base_dir=".", const Value& value=Value::error("null")): Broker({{"typeName", Value{"HTTPBroker"}}}),
    server_(nerikiri::server()), address_(address), port_(port), baseDirectory_(base_dir)
  {
    logger::trace("HTTPBroker::HTTPBroker()");

    if (!value.isError()) {
      value.object_for_each([this](auto k, auto v) {
        route_[k] = v.stringValue();
      });
    }
  }

  virtual ~HTTPBroker() {
    logger::trace("HTTPBroker::~HTTPBroker()");
  }

  nerikiri::Response response(std::function<Value(void)> func) {
    try {
      return nerikiri::Response(200, nerikiri::json::toJSONString(func()), "application/json");
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

  bool run(Process* process) override {
    std::unique_lock<std::mutex> lock(mutex_);
    logger::trace("HTTPBroker::run()");

    server_->baseDirectory(baseDirectory_);

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

    server_->response("/process/.*", "GET", "text/html", [this, process](const nerikiri::Request& req) -> nerikiri::Response {
      logger::trace("HTTPBroker::Response(url='{}')", req.matches[0]);
      return response([process, &req](){
        return process->coreBroker()->readResource(req.matches[0]);
      });
    });

    server_->response("/process/.*", "POST", "text/html", [this, process](const nerikiri::Request& req) -> nerikiri::Response {
      logger::trace("HTTPBroker::Response(url='{}')", req.matches[0]);
      return response([this, process, &req](){
        return process->coreBroker()->createResource(req.matches[0], nerikiri::json::toValue(req.body));
      });
    });

    server_->response("/process/.*", "DELETE", "text/html", [this, process](const nerikiri::Request& req) -> nerikiri::Response {
      return response([this, process, &req](){
        return process->coreBroker()->deleteResource(req.matches[0]);
      });
    });

    server_->response("/process/.*", "PUT", "text/html", [this, &process](const nerikiri::Request& req) -> nerikiri::Response {
      return response([this, &process, &req](){
        return process->coreBroker()->updateResource(req.matches[0], nerikiri::json::toValue(req.body)); 
      });
    });


    for(auto [k, v] : route_) {
      const std::string path = v;
      server_->response(k, "GET", "text/html", [this, path, &process](const nerikiri::Request& req) -> nerikiri::Response {
        const std::string relpath = req.matches[1];
        return nerikiri::Response(path + relpath);
      });
    }

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

/**
 * 
 * 
 */
class HTTPBrokerProxyImpl : public AbstractBrokerProxy {
private:
  nerikiri::HttpClient_ptr client_;
public:
  HTTPBrokerProxyImpl(const std::string& addr, const int64_t port) : AbstractBrokerProxy({ {"host", addr}, {"port", Value(port)} }), client_(nerikiri::client(addr, port)) {
    client_->setTimeout(0.5);
  }

  virtual ~HTTPBrokerProxyImpl() {}

private:
  Value toValue(nerikiri::Response&& res) const {  
    if (res.status != 200) {
      logger::error("HTTBrokerProxyImpl: request failed. status = {}", res.status);
      logger::error("body - {}", res.body);
      return nerikiri::Value::error(res.body);
    }
    try {
      return nerikiri::json::toValue(res.body);
    } catch (nerikiri::json::JSONParseError& e) {
      logger::error("JSON Parse Error: \"{}\"", e.what());
      return nerikiri::Value::error(e.what());
    } catch (nerikiri::json::JSONConstructError& e) {
      logger::error("JSON Construct Error: \"{}\"", e.what());
      return nerikiri::Value::error(e.what());
    }
  }
public:


    virtual Value createResource(const std::string& path, const Value& value) override {
      logger::debug("HTTPBrokerProxyImpl::createResource({})", path);
      return toValue(client_->request(path, "POST", {"POST", nerikiri::json::toJSONString(value), "application/json"}));
    }

    virtual Value readResource(const std::string& path) const override {
      logger::debug("HTTPBrokerProxyImpl::readResource({})", path);
      return toValue(client_->request(path, "GET"));
    }

    virtual Value updateResource(const std::string& path, const Value& value) override {
      logger::debug("HTTPBrokerProxyImpl::updateResource({})", path);
      return toValue(client_->request(path, "PUT", {"PUT", nerikiri::json::toJSONString(value), "application/json"}));
    }

    virtual Value deleteResource(const std::string& path) override {
      logger::debug("HTTPBrokerProxyImpl::deleteResource({})", path);
      return toValue(client_->request(path, "DELETE"));
    }
};


/**
 * 
 * 
 */
class HTTPBrokerFactory : public nerikiri::BrokerFactory {
public:

  HTTPBrokerFactory(): BrokerFactory({{"typeName", "HTTPBroker"}}) {}
  virtual ~HTTPBrokerFactory() {}

public:
  virtual std::shared_ptr<Broker> create(const Value& value)  {
    auto address = value.at("host").stringValue();
    auto port = value.at("port").intValue();
    std::string base_dir = ".";
    if (value.hasKey("baseDir")) {
      base_dir = value.at("baseDir").stringValue();
    }
    return std::dynamic_pointer_cast<nerikiri::Broker>(std::make_shared<HTTPBroker>(address, port, base_dir, value.at("route")));
  }
  
  virtual std::shared_ptr<BrokerAPI> createProxy(const Value& value) {
    logger::trace("HTTPBrokerFactory::createProxy({})", value);
    if (value.hasKey("host") && value.hasKey("port")) {
      auto address = value.at("host").stringValue();
      auto port = value.at("port").intValue();
      return std::dynamic_pointer_cast<nerikiri::BrokerAPI>(std::make_shared<HTTPBrokerProxyImpl>(address, port));
    }
    logger::error("HTTPBrokerFactory::createProxy({}). Failed. Argument value does not have 'host' or 'port' value.");
    return std::make_shared<NullBrokerProxy>();
  }

};


void* createHTTPBroker() {
    return new HTTPBrokerFactory();
}
