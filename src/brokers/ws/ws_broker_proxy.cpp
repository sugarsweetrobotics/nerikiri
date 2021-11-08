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
class WSBrokerProxy : public juiz::CRUDBrokerProxyBase {
private:
  const std::string endpoint_;
  const std::string address_;
  const int64_t port_;
  WebSocketClient_ptr client_;
public:
  WSBrokerProxy(const std::string& addr, const int64_t port) : CRUDBrokerProxyBase("WSBrokerProxy", "WSBroker", "wsBrokerProxy"), 
    client_(juiz::ws::client(addr, port)), endpoint_("wsBroker"), address_(addr), port_(port)
  {
    client_->setTimeout(0.5);
  }

  virtual ~WSBrokerProxy() {}


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
      logger::error("WSBrokerProxyImpl: request failed. status = {}", res.status);
      logger::error("body - {}", res.body);
      return std::move(res.body);
    }
    try {
      return std::move(res.body);
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
      logger::debug("WSBrokerProxyImpl::createResource({})", path);
      return toValue(client_->request("/" + endpoint_ + "/" + path, "POST", {path, "POST", value}));
    }

    virtual Value readResource(const std::string& path) const override {
      logger::debug("WSBrokerProxyImpl(addr={}, port={})::readResource({})", address_, (int32_t)port_, path);
      return toValue(client_->request("/" + endpoint_ + "/" + path, "GET"));
    }

    virtual Value updateResource(const std::string& path, const Value& value) override {
      logger::debug("WSBrokerProxyImpl(addr={}, port={})::updateResource({})", address_, (int32_t)port_, path);
      // return toValue(client_->request("/" + endpoint_ + "/" + path, "PUT", {"PUT", juiz::json::toJSONString(value), "application/json"}));
      return toValue(client_->request("/" + endpoint_ + "/" + path, "PUT", {path, "PUT", (value)}));
    }

    virtual Value deleteResource(const std::string& path) override {
      logger::debug("WSBrokerProxyImpl::deleteResource({})", path);
      return toValue(client_->request("/" + endpoint_ + "/" + path, "DELETE"));
    }
};


std::shared_ptr<ClientProxyAPI> WebSocketBrokerFactory::createProxy(const Value& value) {
  logger::trace("WSBrokerFactory::createProxy({})", value);
  if (value.hasKey("host") && value.hasKey("port")) {
      auto address = value.at("host").stringValue();
      auto port = value.at("port").intValue();
      return std::make_shared<WSBrokerProxy>(address, port);
  }
  logger::error("WSBrokerFactory::createProxy({}). Failed. Argument value does not have 'host' or 'port' value.");
  return nullBrokerProxy();
}