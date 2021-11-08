#include <cstdlib>
#include <string>
#include <condition_variable>
#include <fstream>
#include <map>

#include <juiz/crud_broker.h>
#include <juiz/broker_api.h>

#include "ws.h"
#include "ws_server.h"
#include "ws_broker_factory.h"
using namespace juiz;
using namespace juiz::ws;

extern "C" {
    JUIZ_OPERATION  void* createWSBroker();
};

/**
 * 
 * 
 */
class WSBroker : public CRUDBrokerBase {
private:
  juiz::ws::WSServer_ptr server_;
  std::string address_;
  std::string allow_;
  int32_t port_;
  std::string baseDirectory_;
  std::condition_variable cond_;
  std::mutex mutex_;
  std::map<std::string, std::string> route_;
  std::string endpoint_;
public:

  WSBroker(const std::string& fullName, const std::string& address, const int32_t port, const std::string& allow, const std::string& base_dir=".", const Value& value=Value::error("null")):
    CRUDBrokerBase("WSBroker", "WSBroker", fullName),
    server_(juiz::ws::server()), address_(address), port_(port), allow_(allow), baseDirectory_(base_dir), endpoint_("wsBroker")
  {
    logger::trace("WSBroker::WSBroker(fullName={}, address={}, port={}) called", fullName, address, port);

    if (!value.isError()) {
      value.const_object_for_each([this](auto k, auto v) {
        route_[k] = v.stringValue();
      });
    }
  }

  virtual ~WSBroker() {
    logger::trace("WSBroker::~WSBroker()");
  }

  virtual std::string scheme() const override {
      return "ws://";
  }

  virtual std::string domain() const override {
      return address_ + ":" + std::to_string(port_) + "/";
  }

  juiz::ws::Response toResponse(const Value& value) const {
    try {
        return juiz::ws::Response(200, value, "application/json_hoge");
    } catch (juiz::json::JSONParseError& e) {
        logger::error("JSON Parse Error in HTTPBroker::response(): \"{}\"", e.what());
        return juiz::ws::Response(400, Value::error("Bad Request"), "text/html");
    } catch (juiz::json::JSONConstructError& e) {
        logger::error("JSON Construct Error in HTTPBroker::response(): \"{}\"", e.what());
        return juiz::ws::Response(400, Value::error(e.what()), "text/html");
    } catch (juiz::ValueTypeError &e) {
        logger::error("ValueTypeError in HTTPBroker::response(): \"{}\"", e.what());
        return juiz::ws::Response(400, Value::error(e.what()), "text/html");
    } catch (std::exception &e) {
        logger::error("Exception in HTTPBroker::response(): \"{}\"", e.what());
        return juiz::ws::Response(400, Value::error(e.what()), "text/html");
    }
  }

  juiz::Value toValue(const std::string& body) {
    try {
      return juiz::json::toValue(body);
    } catch (juiz::json::JSONParseError& e) {
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

  Value headerParam(const juiz::ws::Request& req) {
    auto param = Value::object();
    for(auto h : req.headers) {
      param[h.first] = h.second;
    }
    return param;
  }
  
  Value hostInfo(const juiz::ws::Request& req) {
    auto info = this->fullInfo();
    auto param = Value::object();
    for(auto h : req.headers) {
      param[h.first] = h.second;
    }
    auto host = Value::string(param["Host"], "");
    if (host.find(':') != std::string::npos) {
      host = host.substr(0, host.find(':'));
    }
    info["host"] = host;
    return info;
  }

  virtual bool run(const std::shared_ptr<ClientProxyAPI>& coreBroker) override {
    std::unique_lock<std::mutex> lock(mutex_);
    logger::info("WSBroker::run() called");

    server_->baseDirectory(baseDirectory_);


    const std::string endpoint = "/" + endpoint_ + "/(.*)";

    server_->response(endpoint, "GET", "text/html", [this, coreBroker](const juiz::ws::Request& req) -> juiz::ws::Response {
      logger::trace("WSBroker::Response(method='GET', url='{}')", req.path);
      return toResponse(onRead(coreBroker, req.path, headerParam(req), this->hostInfo(req)));
    });

    server_->response(endpoint, "POST", "text/html", [this, coreBroker](const juiz::ws::Request& req) -> juiz::ws::Response {
      logger::trace("WSBroker::Response(method='POST', url='{}')", req.path);
      return toResponse(onCreate(coreBroker, req.path, req.body));
    });

    server_->response(endpoint, "DELETE", "text/html", [this, coreBroker](const juiz::ws::Request& req) -> juiz::ws::Response {
      logger::trace("WSBroker::Response(method='DELETE', url='{}')", req.path);
      return toResponse(onDelete(coreBroker, req.path));
    });

    server_->response(endpoint, "PUT", "text/html", [this, &coreBroker](const juiz::ws::Request& req) -> juiz::ws::Response {
      logger::trace("WSBroker::Response(method='PUT', url='{}')", req.path);
      return toResponse(onUpdate(coreBroker, req.path, req.body));
    });


    for(auto [k, v] : route_) {
      const std::string path = v;
      server_->response(k, "GET", "text/html", [this, path, &coreBroker](const juiz::ws::Request& req) -> juiz::ws::Response {
        logger::trace("WSBroker::Response(method='GET', url='{}')", req.matches[1]);
        const std::string relpath = req.matches[1];
        return juiz::ws::Response(path + relpath);
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


std::shared_ptr<BrokerAPI> WebSocketBrokerFactory::create(const Value& value) {
    if (!value.hasKey("host")) {
        logger::error("WSBrokerFactory::create({}) failed. There is no 'host' value.", value);
        return nullBroker();
    }
    if (!value["host"].isStringValue()) {
      logger::error("WSBrokerFactory::create({}) failed. 'host' option must be string value. Failed.", value);
      return nullBroker();
    }
    
    
    if (value.hasKey("allow") && !value["allow"].isStringValue()) {
      logger::error("WSBrokerFactory::create({}) failed. 'allow' option must be string value. Failed.", value);
      return nullBroker();
    }

    if (!value.hasKey("fullName")) {
        logger::error("WSBrokerFactory::create({}) failed. There is no 'fullName' value.", value);
        return nullBroker();
    }
    if (!value["fullName"].isStringValue()) {
      logger::error("WSPBrokerFactory::create({}) failed. 'fullName' option must be string value. Failed.", value);
      return nullBroker();
    }
    
    if (!value.hasKey("port")) {
        logger::error("WSBrokerFactory::create({}) failed. There is no 'port' value.", value);
            return nullBroker();
    }
    if (!value["port"].isIntValue()) {
      logger::error("WSBrokerFactory::create({}) failed. 'port' option must be integer value. Failed.", value);
      return nullBroker();
    }

    std::string base_dir = ".";
    if (value.hasKey("baseDir")) {
        if (!value["baseDir"].isStringValue()) {
            logger::warn("WSBrokerFactory::create({}) warning. 'baseDir' option must be string value. Ignored.", value);
        }
        base_dir = value.at("baseDir").stringValue();
    }
    Value routeInfo = {};
    if (value.hasKey("route")) {
        if (!value["route"].isStringValue()) {
            logger::warn("WSBrokerFactory::create({}) warning. 'route' option must be string value. Ignored.", value);
        }
        routeInfo = value.at("route");
    }
    
    return std::make_shared<WSBroker>(value["fullName"].stringValue(), value["host"].stringValue(), value["port"].intValue(), Value::string(value["allow"], "0.0.0.0"), base_dir, routeInfo);
}


void* createWSBroker() {
    return new WebSocketBrokerFactory("wsBroker");
}
