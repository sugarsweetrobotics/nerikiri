#include <cstdlib>

#include "nerikiri/nerikiri.h"
#include "nerikiri/logger.h"
#include "nerikiri/datatype/json.h"
#include "nerikiri/http/httpbroker.h"
#include "webi/http_server.h"
#include "nerikiri/process.h"
#include "webi/http_client.h"

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
  //nerikiri::Process_ptr process_;
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

  bool run() override {
    std::unique_lock<std::mutex> lock(mutex_);
    
    logger::trace("HTTPBroker::run()");
    server_->response("/broker/info", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
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

            info["address"] = Value(addr);
            info["port"] = Value(port);
          }
        }
        return info;
        });
    });
    server_->response("/process/info", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this](){return Broker::getProcessInfo();});
    });
    server_->response("/process/operations", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this](){return Broker::getOperationInfos();});
    }); 
    server_->response("/process/containers", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this](){return Broker::getContainerInfos();});
    });
    server_->response("/process/container/([^/]*)", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::getContainerInfo({{"name", Value(req.matches[1])}});});
    });
    server_->response("/process/container/([^/]*)/operations", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::getContainerOperationInfos({{"name", Value(req.matches[1])}});});
    });
    server_->response("/process/container/([^/]*)/operation/([^/]*)", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::getContainerOperationInfo(
        {{"name", Value(req.matches[1])}}, {{"name", Value(req.matches[2])}});});
    });
    server_->response("/process/container/([^/]*)/operation/([^/]*)/call", "PUT", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::callContainerOperation(
        {{"name", Value(req.matches[1])}}, {{"name", Value(req.matches[2])}}, nerikiri::json::toValue(req.body));});
    });
    server_->response("/process/container/([^/]*)/operation/([^/]*)/invoke", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::invokeContainerOperation(
        {{"name", Value(req.matches[1])}}, {{"name", Value(req.matches[2])}});});
    });
    server_->response("/process/operation/([^/]*)", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::getOperationInfo({{"name", Value(req.matches[1])}});});
    });
    server_->response("/process/operation/([^\\/]*)/call", "PUT", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::callOperation({{"name", Value(req.matches[1])}}, nerikiri::json::toValue(req.body));});
    });
    server_->response("/process/operation/([^\\/]*)/invoke", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::invokeOperationByName(req.matches[1]);});
    });
    server_->response("/process/make_connection", "POST", "application/json", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return nerikiri::makeConnection(this, nerikiri::json::toValue(req.body));});
    });
    server_->response("/process/register_consumer_connection", "POST", "application/json", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::registerConsumerConnection(nerikiri::json::toValue(req.body));});
    });
    server_->response("/process/remove_consumer_connection", "POST", "application/json", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::removeConsumerConnection(nerikiri::json::toValue(req.body));});
    });
    server_->response("/process/operation/([^\\/]*)/argument/([^\\/]*)/([-/]*)/push", "PUT", "application/json", [this](const webi::Request& req) -> webi::Response {
      std::string operation_name = req.matches[1];//ci.at("consumer").at("info").at("name").stringValue();
      std::string argument_name  = req.matches[2];//ci.at("consumer").at("target").at("name").stringValue();
      std::string connection_name = req.matches[3];//ci.at("name").stringValue();
      Value info = { 
        {"name", connection_name}, 
        {"consumer", { 
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

  void shutdown() override {
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
    virtual BrokerInfo getBrokerInfo() const override {
      return toValue(client_->request("/broker/info", "GET"));
    }

    virtual Value getProcessInfo() const override {
      return toValue(client_->request("/process/info", "GET"));
    }

    virtual Value getOperationInfos() const override {
      return toValue(client_->request("/process/operations", "GET"));
    }

    virtual Value getContainerInfos() const override {
      return toValue(client_->request("/process/containers", "GET"));
    }

    virtual Value getContainerInfo(const Value& v) const override {
      return toValue(client_->request("/process/container/" + v.at("name").stringValue(), "GET"));
    }

    virtual Value getContainerOperationInfos(const Value& v) const override {
      return toValue(client_->request("/process/container/" + v.at("name").stringValue() + "/operations", "GET"));
    }

    virtual Value getContainerOperationInfo(const Value& ci, const Value& oi) const override {
      return toValue(client_->request("/process/container/" + ci.at("name").stringValue() + "/operation/" + oi.at("name").stringValue(), "GET"));
    }

    virtual Value callContainerOperation(const Value& ci, const Value& oi, Value&& arg) const override {
     return toValue(client_->request("/process/container/" + ci.at("name").stringValue() + "/operation/" + oi.at("name").stringValue() + "/call", "PUT"));
    }

    virtual Value invokeContainerOperation(const Value& ci, const Value& oi) const override {
     return toValue(client_->request("/process/container/" + ci.at("name").stringValue() + "/operation/" + oi.at("name").stringValue() + "/invoke", "GET"));
    }

    virtual Value getContainerInfoByName(const std::string& name) const {
      return toValue(client_->request("/process/container/" + name, "GET"));
    }

    virtual Value getOperationInfo(const Value& v) const override {
      return toValue(client_->request("/process/operation/" + v.at("name").stringValue(), "GET"));
    }

    virtual Value getOperationInfoByName(const std::string& name) const {
      return toValue(client_->request("/process/operation/" + name, "GET"));
    }

    virtual Value callOperationByName(const std::string& name, Value&& value) const {
      return toValue(client_->request("/process/operation/" + name + "/call", "POST"));
    }

    virtual Value callOperation(const Value& info, Value&& value) const override {
      return toValue(client_->request("/process/operation/" + info.at("name").stringValue() + "/call", "POST", {"POST", nerikiri::json::toJSONString(value), "application/json"}));
    }

    virtual Value invokeOperationByName(const std::string& name) const override {
      return toValue(client_->request("/process/operation/" + name + "/invoke", "GET"));
    }

    virtual Value makeConnection(const ConnectionInfo& ci) const override {
      return toValue(client_->request("/process/make_connection", "POST", {"POST", nerikiri::json::toJSONString(ci), "application/json"}));
    }

    virtual Value registerConsumerConnection(const ConnectionInfo& ci) const override {
      return toValue(client_->request("/process/register_consumer_connection", "POST", {"POST", nerikiri::json::toJSONString(ci), "application/json"}));
    }

    virtual Value removeConsumerConnection(const ConnectionInfo& ci) const override {
      return toValue(client_->request("/process/remove_consumer_connection", "POST", {"POST", nerikiri::json::toJSONString(ci), "application/json"}));
    }

    virtual Value pushViaConnection(const ConnectionInfo& ci, Value&& value)  const override {
      auto operation_name = ci.at("consumer").at("info").at("name").stringValue();
      auto argument_name  = ci.at("consumer").at("target").at("name").stringValue();
      auto connection_name = ci.at("name").stringValue();
      return toValue(client_->request("/process/operation/" + operation_name + "/argument/" + argument_name + "/" + connection_name + "/push", "PUT", {"PUT", nerikiri::json::toJSONString(value), "application/json"}));
    }
};

nerikiri::Broker_ptr nerikiri::http::brokerProxy(const std::string& address, const int32_t port) {
  return nerikiri::Broker_ptr(new HTTPBrokerProxyImpl(address, port));
}
