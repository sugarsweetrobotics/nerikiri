
  
#include <string>
#include <condition_variable>

#include "nerikiri/broker.h"
#include "nerikiri/brokerproxy.h"

#include "nerikiri/brokerfactory.h"
  
using namespace nerikiri;
  
class HTTPBrokerProxy : public AbstractBrokerProxy {
public:
HTTPBrokerProxy(const Value& info) : AbstractBrokerProxy(info) {}
virtual ~HTTPBrokerProxy() {}
};


class HTTPBrokerProxyImpl : public HTTPBrokerProxy {
private:
  webi::HttpClient_ptr client_;
public:
  HTTPBrokerProxyImpl(const std::string& addr, const int64_t port) : HTTPBrokerProxy({ {"host", addr}, {"port", Value(port)} }), client_(webi::client(addr, port)) {
    client_->setTimeout(1);
  }

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
