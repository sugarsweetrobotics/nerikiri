#include <cstdlib>
#include <string>
#include <condition_variable>
#include <fstream>
#include <map>

#include <nerikiri/crud_broker.h>
#include <nerikiri/broker_api.h>
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
  std::string allow_;
  int32_t port_;
  std::string baseDirectory_;
  std::condition_variable cond_;
  std::mutex mutex_;
  std::map<std::string, std::string> route_;
  std::string endpoint_;
public:

  HTTPBroker(const std::string& fullName, const std::string& address, const int32_t port, const std::string& allow, const std::string& base_dir=".", const Value& value=Value::error("null")):
    CRUDBrokerBase("HTTPBroker", "HTTPBroker", fullName),
    server_(nerikiri::server()), address_(address), port_(port), allow_(allow), baseDirectory_(base_dir), endpoint_("httpBroker")
  {
    logger::trace("HTTPBroker::HTTPBroker(fullName={}, address={}, port={}) called", fullName, address, port);

    if (!value.isError()) {
      value.const_object_for_each([this](auto k, auto v) {
        route_[k] = v.stringValue();
      });
    }
  }

  virtual ~HTTPBroker() {
    logger::trace("HTTPBroker::~HTTPBroker()");
  }

  virtual std::string scheme() const override {
      return "http://";
  }

  virtual std::string domain() const override {
      return address_ + ":" + std::to_string(port_) + "/";
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
    try {
      return nerikiri::json::toValue(body);
    } catch (nerikiri::json::JSONParseError& e) {
      return Value::error(logger::error("nerikir::toValue(\"{}\") failed. JSONParseError", body));
    }
  }
  
  virtual Value fullInfo() const override {
    auto i = info();
    i["port"] = port_;
    i["host"] = address_;
    i["allow"] = allow_;
    i["baseDirectory"] = baseDirectory_;
    return i;
  }

  Value headerParam(const nerikiri::Request& req) {
    auto param = Value::object();
    for(auto h : req.headers) {
      param[h.first] = h.second;
    }
    return param;
  }
  
  Value hostInfo(const nerikiri::Request& req) {
    auto info = this->fullInfo();
    auto param = Value::object();
    for(auto h : req.headers) {
      param[h.first] = h.second;
    }
    auto host = Value::string(param["Host"]);
    if (host.find(':') != std::string::npos) {
      host = host.substr(0, host.find(':'));
    }
    info["host"] = host;
    return info;
  }

  virtual bool run(const std::shared_ptr<ClientProxyAPI>& coreBroker) override {
    std::unique_lock<std::mutex> lock(mutex_);
    logger::info("HTTPBroker::run() called");

    server_->baseDirectory(baseDirectory_);


    const std::string endpoint = "/" + endpoint_ + "/(.*)";

    server_->response(endpoint, "GET", "text/html", [this, coreBroker](const nerikiri::Request& req) -> nerikiri::Response {
      logger::trace("HTTPBroker::Response(method='GET', url='{}')", req.matches[1]);
      return toResponse(onRead(coreBroker, req.matches[1], headerParam(req), this->hostInfo(req)));
    });

    server_->response(endpoint, "POST", "text/html", [this, coreBroker](const nerikiri::Request& req) -> nerikiri::Response {
      logger::trace("HTTPBroker::Response(method='POST', url='{}')", req.matches[1]);
      return toResponse(onCreate(coreBroker, req.matches[1], toValue(req.body)));
    });

    server_->response(endpoint, "DELETE", "text/html", [this, coreBroker](const nerikiri::Request& req) -> nerikiri::Response {
      logger::trace("HTTPBroker::Response(method='DELETE', url='{}')", req.matches[1]);
      return toResponse(onDelete(coreBroker, req.matches[1]));
    });

    server_->response(endpoint, "PUT", "text/html", [this, &coreBroker](const nerikiri::Request& req) -> nerikiri::Response {
      logger::trace("HTTPBroker::Response(method='PUT', url='{}')", req.matches[1]);
      return toResponse(onUpdate(coreBroker, req.matches[1], toValue(req.body)));
    });


    for(auto [k, v] : route_) {
      const std::string path = v;
      server_->response(k, "GET", "text/html", [this, path, &coreBroker](const nerikiri::Request& req) -> nerikiri::Response {
        logger::trace("HTTPBroker::Response(method='GET', url='{}')", req.matches[1]);
        const std::string relpath = req.matches[1];
        return nerikiri::Response(path + relpath);
      });
    }

    CRUDBrokerBase::run(coreBroker);
    server_->runBackground(port_);
    cond_.wait(lock);
    
    server_->terminateBackground();
    return true;
  }

  virtual void shutdown(const std::shared_ptr<ClientProxyAPI>& coreBroker) override {
    CRUDBrokerBase::shutdown(coreBroker);
    cond_.notify_all();
  }

};


std::shared_ptr<BrokerAPI> HTTPBrokerFactory::create(const Value& value) {
    if (!value.hasKey("host")) {
        logger::error("HTTPBrokerFactory::create({}) failed. There is no 'host' value.", value);
        return nullBroker();
    }
    if (!value["host"].isStringValue()) {
      logger::error("HTTPBrokerFactory::create({}) failed. 'host' option must be string value. Failed.", value);
      return nullBroker();
    }
    
    
    if (value.hasKey("allow") && !value["allow"].isStringValue()) {
      logger::error("HTTPBrokerFactory::create({}) failed. 'allow' option must be string value. Failed.", value);
      return nullBroker();
    }

    if (!value.hasKey("fullName")) {
        logger::error("HTTPBrokerFactory::create({}) failed. There is no 'fullName' value.", value);
        return nullBroker();
    }
    if (!value["fullName"].isStringValue()) {
      logger::error("HTTPBrokerFactory::create({}) failed. 'fullName' option must be string value. Failed.", value);
      return nullBroker();
    }
    
    if (!value.hasKey("port")) {
        logger::error("HTTPBrokerFactory::create({}) failed. There is no 'port' value.", value);
            return nullBroker();
    }
    if (!value["port"].isIntValue()) {
      logger::error("HTTPBrokerFactory::create({}) failed. 'port' option must be integer value. Failed.", value);
      return nullBroker();
    }

    std::string base_dir = ".";
    if (value.hasKey("baseDir")) {
        if (!value["baseDir"].isStringValue()) {
            logger::warn("HTTPBrokerFactory::create({}) warning. 'baseDir' option must be string value. Ignored.", value);
        }
        base_dir = value.at("baseDir").stringValue();
    }
    Value routeInfo = {};
    if (value.hasKey("route")) {
        if (!value["route"].isStringValue()) {
            logger::warn("HTTPBrokerFactory::create({}) warning. 'route' option must be string value. Ignored.", value);
        }
        routeInfo = value.at("route");
    }
    
    return std::make_shared<HTTPBroker>(value["fullName"].stringValue(), value["host"].stringValue(), value["port"].intValue(), Value::string(value["allow"], "0.0.0.0"), base_dir, routeInfo);
}


void* createHTTPBroker() {
    return new HTTPBrokerFactory("httpBroker");
}
