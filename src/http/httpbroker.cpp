#include <cstdlib>

#include "nerikiri/nerikiri.h"
#include "nerikiri/logger.h"
#include "nerikiri/datatype/json.h"
#include "nerikiri/http/httpbroker.h"
#include "webi/http_server.h"
#include "nerikiri/process.h"
#include "webi/http_client.h"
#include "nerikiri/objectmapper.h"

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
  webi::HttpServer_ptr server_;
  std::condition_variable cond_;
  std::mutex mutex_;
  std::string address_;
  int32_t port_;
public:
  HTTPBrokerImpl(const std::string& address, const int32_t port): HTTPBroker(), server_(webi::server()), address_(address), port_(port) {
    logger::trace("HTTPBroker::HTTPBroker({})", str(getBrokerInfo()));
  }
  virtual ~HTTPBrokerImpl() {
    logger::trace("HTTPBroker::~HTTPBroker()");
  }

  webi::Response response(std::function<const Value(void)> func) {
    try {
      return webi::Response(200, nerikiri::json::toJSONString(func()), "application/json");
    } catch (nerikiri::json::JSONParseError& e) {
      logger::error("JSON Parse Error: \"{}\"", e.what());
      return webi::Response(400, "Bad Request", "text/html");
    } catch (nerikiri::json::JSONConstructError& e) {
      logger::error("JSON Construct Error: \"{}\"", e.what());
      return webi::Response(400, e.what(), "text/html");
    }
  }

  bool run(Process* process) override {
    std::unique_lock<std::mutex> lock(mutex_);
    
    logger::trace("HTTPBroker::run()");

    server_->response("/broker/info/", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){
        auto info = Broker::getBrokerInfo();
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

            info["host"] = Value(addr);
            info["port"] = Value(port);
          }
        }
        return info;
        });
    });
    server_->response("/.*", "GET", "text/html", [this, process](const webi::Request& req) -> webi::Response {
      logger::trace("HTTPBroker::Response(url='{}')", req.matches[0]);
      return response([process, &req](){return process->requestResource(req.matches[0]);});
    });

    server_->response("/.*", "POST", "text/html", [this, process](const webi::Request& req) -> webi::Response {
      logger::trace("HTTPBroker::Response(url='{}')", req.matches[0]);
      return response([this, process, &req](){return process->createResource(req.matches[0], nerikiri::json::toValue(req.body), this);});
    });

    server_->response("/.*", "DELETE", "text/html", [this, process](const webi::Request& req) -> webi::Response {
      return response([this, process, &req](){return process->deleteResource(req.matches[0], this);});
    });
    
    server_->response("/process/container/([^/]*)/operation/([^/]*)/call", "PUT", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::callContainerOperation(
        {{"name", Value(req.matches[1])}}, {{"name", Value(req.matches[2])}}, nerikiri::json::toValue(req.body));});
    });

    server_->response("/process/operation/([^\\/]*)/call", "PUT", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::callOperation({{"name", Value(req.matches[1])}}, nerikiri::json::toValue(req.body));});
    });
    
    server_->response("/process/operation/([^/]*)/argument/([^/]*)/([^/]*)/push", "PUT", "application/json", [this](const webi::Request& req) -> webi::Response {
      std::string operation_name = req.matches[1];
      std::string argument_name  = req.matches[2];
      std::string connection_name = req.matches[3];
      Value info = { 
        {"name", connection_name}, 
        {"input", { 
          {"info", { {"name", operation_name} } }, 
          {"target", { {"name", argument_name} } }
          }
        }};
      return response([this, info, &req](){return Broker::pushViaConnection(info, nerikiri::json::toValue(req.body));});
    });
    server_->runBackground(port_);
    cond_.wait(lock);
    
    server_->terminateBackground();
    return true;
  }

  void shutdown(Process* proc) override {
    cond_.notify_all();
  }

};

nerikiri::Broker_ptr nerikiri::http::broker(const std::string& address, const int32_t port) {
  return nerikiri::Broker_ptr(new HTTPBrokerImpl(address, port));
}


class HTTPBrokerProxyImpl : public HTTPBrokerProxy {
private:
  webi::HttpClient_ptr client_;
public:
  HTTPBrokerProxyImpl(const std::string& addr, const int port) : client_(webi::client(addr, port)) {}

  virtual ~HTTPBrokerProxyImpl() {}

private:
  Value toValue(webi::Response&& res) const {  
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

    virtual Value callContainerOperation(const Value& ci, const Value& oi, Value&& arg) const override {
     return toValue(client_->request("/process/container/" + ci.at("name").stringValue() + "/operation/" + oi.at("name").stringValue() + "/call", "PUT"));
    }

    virtual Value callOperationByName(const std::string& name, Value&& value) const {
      return toValue(client_->request("/process/operation/" + name + "/call", "POST"));
    }

    virtual Value callOperation(const Value& info, Value&& value) const override {
      return toValue(client_->request("/process/operation/" + info.at("name").stringValue() + "/call", "POST", {"POST", nerikiri::json::toJSONString(value), "application/json"}));
    }


    virtual Value removeProviderConnection(const ConnectionInfo& ci) override {
      return toValue(client_->request("/process/remove_consumer_connection", "DELETE", {"DELETE", nerikiri::json::toJSONString(ci), "application/json"}));
    }

    virtual Value pushViaConnection(const ConnectionInfo& ci, Value&& value)  const override {
      auto operation_name = ci.at("input").at("info").at("name").stringValue();
      auto argument_name  = ci.at("input").at("target").at("name").stringValue();
      auto connection_name = ci.at("name").stringValue();
      return toValue(client_->request("/process/operation/" + operation_name + "/argument/" + argument_name + "/" + connection_name + "/push", "PUT", {"PUT", nerikiri::json::toJSONString(value), "application/json"}));
    }

    virtual Value requestResource(const std::string& path) const override {
      return toValue(client_->request(path, "GET"));
    }

    virtual Value createResource(const std::string& path, const Value& value) override {
      return toValue(client_->request(path, "POST", {"POST", nerikiri::json::toJSONString(value), "application/json"}));
    }

    virtual Value deleteResource(const std::string& path) override {
      return toValue(client_->request(path, "DELETE"));
    }
};

nerikiri::Broker_ptr nerikiri::http::brokerProxy(const std::string& address, const int32_t port) {
  return nerikiri::Broker_ptr(new HTTPBrokerProxyImpl(address, port));
}

std::shared_ptr<nerikiri::BrokerAPI> nerikiri::http::HTTPBrokerFactory::create(const Value& value) {
  auto address = value.at("host").stringValue();
  auto port = value.at("port").intValue();
  return nerikiri::Broker_ptr(new HTTPBrokerImpl(address, port));
}

std::shared_ptr<nerikiri::BrokerAPI> nerikiri::http::HTTPBrokerFactory::createProxy(const Value& value) {
  auto address = value.at("host").stringValue();
  auto port = value.at("port").intValue();
  return nerikiri::Broker_ptr(new HTTPBrokerProxyImpl(address, port));
}