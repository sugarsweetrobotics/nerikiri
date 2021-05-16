#include <cstdlib>
#include <string>
#include <condition_variable>
#include <fstream>
#include <map>

#include "http_broker_factory.h"

#include <juiz/crud_broker_proxy_base.h>
#include <juiz/utils/json.h>
#include "http_client.h"
#include "http_client.h"
#include "http_server.h"


using namespace juiz;
/**
 * 
 * 
 */
class HTTPBrokerProxy : public juiz::CRUDBrokerProxyBase {
private:
  juiz::HttpClient_ptr client_;
  const std::string endpoint_;
  const std::string address_;
  const int64_t port_;
public:
  HTTPBrokerProxy(const std::string& addr, const int64_t port) : CRUDBrokerProxyBase("HTTPBrokerProxy", "HTTPBroker", "httpBrokerProxy"), 
    client_(juiz::client(addr, port)), endpoint_("httpBroker"), address_(addr), port_(port)
  {
    client_->setTimeout(0.5);
  }

  virtual ~HTTPBrokerProxy() {}


  virtual std::string scheme() const override {
      return "http://";
  }

  virtual std::string domain() const override {
      return address_ + ":" + std::to_string(port_) + "/";
  }

public:
  virtual Value info() const override {
    auto i = juiz::CRUDBrokerProxyBase::info();
    i["port"] = port_;
    i["host"] = address_;
    return i;
  }
private:
  Value toValue(juiz::Response&& res) const {  
    if (res.status != 200) {
      logger::error("HTTBrokerProxyImpl: request failed. status = {}", res.status);
      logger::error("body - {}", res.body);
      return juiz::Value::error(res.body);
    }
    try {
      return juiz::json::toValue(res.body);
    } catch (juiz::json::JSONParseError& e) {
      logger::error("JSON Parse Error: \"{}\"", e.what());
      return juiz::Value::error(e.what());
    } catch (juiz::json::JSONConstructError& e) {
      logger::error("JSON Construct Error: \"{}\"", e.what());
      return juiz::Value::error(e.what());
    }
  }
public:


    virtual Value createResource(const std::string& path, const Value& value) override {
      logger::debug("HTTPBrokerProxyImpl::createResource({})", path);
      return toValue(client_->request("/" + endpoint_ + "/" + path, "POST", {"POST", juiz::json::toJSONString(value), "application/json"}));
    }

    virtual Value readResource(const std::string& path) const override {
      logger::debug("HTTPBrokerProxyImpl(addr={}, port={})::readResource({})", address_, (int32_t)port_, path);
      return toValue(client_->request("/" + endpoint_ + "/" + path, "GET"));
    }

    virtual Value updateResource(const std::string& path, const Value& value) override {
      logger::debug("HTTPBrokerProxyImpl(addr={}, port={})::updateResource({})", address_, (int32_t)port_, path);
      return toValue(client_->request("/" + endpoint_ + "/" + path, "PUT", {"PUT", juiz::json::toJSONString(value), "application/json"}));
    }

    virtual Value deleteResource(const std::string& path) override {
      logger::debug("HTTPBrokerProxyImpl::deleteResource({})", path);
      return toValue(client_->request("/" + endpoint_ + "/" + path, "DELETE"));
    }
};


std::shared_ptr<ClientProxyAPI> HTTPBrokerFactory::createProxy(const Value& value) {
  logger::trace("HTTPBrokerFactory::createProxy({})", value);
  if (value.hasKey("host") && value.hasKey("port")) {
      auto address = value.at("host").stringValue();
      auto port = value.at("port").intValue();
      return std::make_shared<HTTPBrokerProxy>(address, port);
  }
  logger::error("HTTPBrokerFactory::createProxy({}). Failed. Argument value does not have 'host' or 'port' value.");
  return nullBrokerProxy();
}