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
    logger::trace("HTTPBroker::HTTPBroker({})", str(info()));
  }
  virtual ~HTTPBrokerImpl() {
    logger::trace("HTTPBroker::~HTTPBroker()");
  }
  /*
  virtual void setProcess(Process_ptr process) override {
    process_ = process;
  }
  */

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
        auto info = Broker::info();
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


            info["host"] = Value(h.second);
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
      return response([this](){return Broker::getProcessOperationInfos();});
    });
    server_->response("/process/operation/([^/]*)", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::getOperationInfoByName(req.matches[1]);});
    });
    server_->response("/process/operation/([^\\/]*)/call", "PUT", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::callOperationByName(req.matches[1], nerikiri::json::toValue(req.body));});
    });
    server_->response("/process/operation/([^\\/]*)/invoke", "GET", "text/html", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::invokeOperationByName(req.matches[1]);});
    });
    server_->response("/process/make_connection", "POST", "application/json", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::makeConnection(nerikiri::json::toValue(req.body));});
    });
    server_->response("/process/register_connection", "POST", "application/json", [this](const webi::Request& req) -> webi::Response {
      return response([this, &req](){return Broker::registerConnection(nerikiri::json::toValue(req.body));});
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
    virtual BrokerInfo info() const {
      return toValue(client_->request("/broker/info", "GET"));
    }

    virtual Value getProcessInfo() const {
      return toValue(client_->request("/process/info", "GET"));
    }

    virtual Value getProcessOperationInfos() const {
      return toValue(client_->request("/process/operations", "GET"));
    }

    virtual Value getOperationInfoByName(const std::string& name) const {
      return toValue(client_->request("/process/operation/" + name, "GET"));
    }

    virtual Value callOperationByName(const std::string& name, Value&& value) const {

    }

    virtual Value invokeOperationByName(const std::string& name) const {
      return toValue(client_->request("/process/operation/" + name + "/invoke", "GET"));
    }

    virtual Value makeConnection(const ConnectionInfo& ci) const {
      auto name = ci.at("from").at("process").at("name").stringValue();
      return toValue(client_->request("/process/operation/" + name + "/connections/", "POST", {"POST", nerikiri::json::toJSONString(ci), "application/json"}));
    }

    virtual Value registerConnection(const ConnectionInfo& ci) const {
      //return toValue(client_->request("/process/operation/" + name + "/connections/", "POST", {"POST", nerikiri::json::toJSONString(ci), "application/json"}));
    }
};

nerikiri::Broker_ptr nerikiri::http::brokerProxy(const std::string& address, const int32_t port) {
  return nerikiri::Broker_ptr(new HTTPBrokerProxyImpl(address, port));
}
