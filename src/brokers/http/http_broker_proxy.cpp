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


virtual std::shared_ptr<BrokerAPI> HttpBrokerFactory::createProxy(const Value& value) {
logger::trace("HTTPBrokerFactory::createProxy({})", value);
if (value.hasKey("host") && value.hasKey("port")) {
    auto address = value.at("host").stringValue();
    auto port = value.at("port").intValue();
    return std::dynamic_pointer_cast<nerikiri::BrokerAPI>(std::make_shared<HTTPBrokerProxyImpl>(address, port));
}
logger::error("HTTPBrokerFactory::createProxy({}). Failed. Argument value does not have 'host' or 'port' value.");
return std::make_shared<NullBrokerProxy>();
}