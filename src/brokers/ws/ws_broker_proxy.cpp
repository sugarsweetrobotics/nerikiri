#include <cstdlib>
#include <string>
#include <condition_variable>
#include <fstream>
#include <map>

#include "ws_client.h"
#include "ws_broker_factory.h"

#include <juiz/crud_broker_proxy_base.h>
#include <juiz/utils/json.h>

using namespace juiz;
using namespace juiz::ws;
/**
 * 
 * 
 */
class WebSocketBrokerProxy : public juiz::CRUDBrokerProxyBase {
private:
  const std::string endpoint_;
  const std::string address_;
  const int64_t port_;
  WebSocketClient_ptr client_;
public:
  WebSocketBrokerProxy(const std::string& addr, const int64_t port) : CRUDBrokerProxyBase("WebSocketBrokerProxy", "WebSocketBroker", "webSocketBrokerProxy"), 
    client_(juiz::ws::client(addr, port)), endpoint_("webSocketBroker"), address_(addr), port_(port)
  {
    client_->setTimeout(0.5);
  }

  virtual ~WebSocketBrokerProxy() {}


  virtual std::string scheme() const override {
      return "ws://";
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
  Value toValue(juiz::ws::Response&& res) const {  
    if (res.status != 200) {
      logger::error("WebSocketBrokerProxyImpl: request failed. status = {}", res.status);
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
      logger::debug("WebSocketBrokerProxyImpl::createResource({})", path);
      return toValue(client_->request("/" + endpoint_ + "/" + path, "POST", {"POST", juiz::json::toJSONString(value), "application/json"}));
    }

    virtual Value readResource(const std::string& path) const override {
      logger::debug("WebSocketBrokerProxyImpl(addr={}, port={})::readResource({})", address_, (int32_t)port_, path);
      return toValue(client_->request("/" + endpoint_ + "/" + path, "GET"));
    }

    virtual Value updateResource(const std::string& path, const Value& value) override {
      logger::debug("WebSocketBrokerProxyImpl(addr={}, port={})::updateResource({})", address_, (int32_t)port_, path);
      return toValue(client_->request("/" + endpoint_ + "/" + path, "PUT", {"PUT", juiz::json::toJSONString(value), "application/json"}));
    }

    virtual Value deleteResource(const std::string& path) override {
      logger::debug("WebSocketBrokerProxyImpl::deleteResource({})", path);
      return toValue(client_->request("/" + endpoint_ + "/" + path, "DELETE"));
    }
};


std::shared_ptr<ClientProxyAPI> WebSocketBrokerFactory::createProxy(const Value& value) {
  logger::trace("WebSocketBrokerFactory::createProxy({})", value);
  if (value.hasKey("host") && value.hasKey("port")) {
      auto address = value.at("host").stringValue();
      auto port = value.at("port").intValue();
      return std::make_shared<WebSocketBrokerProxy>(address, port);
  }
  logger::error("WebSocketBrokerFactory::createProxy({}). Failed. Argument value does not have 'host' or 'port' value.");
  return nullBrokerProxy();
}