#include <cstdlib>
#include <string>
#include <condition_variable>
#include <fstream>
#include <map>

#include "http_broker_factory.h"

/*
#include "nerikiri/nerikiri.h"
#include "nerikiri/logger.h"
#include "nerikiri/json.h"

#include "nerikiri/process.h"
#include "nerikiri/objectmapper.h"
*/


/*
#include "nerikiri/broker_api.h"
#include "nerikiri/abstractbrokerproxy.h"
#include "nerikiri/broker_factory_api.h"
#include "nerikiri/nullbrokerproxy.h"
*/

#include <nerikiri/crud_broker_proxy_base.h>
#include <nerikiri/json.h>
#include "http_client.h"
#include "http_client.h"
#include "http_server.h"


using namespace nerikiri;
/**
 * 
 * 
 */
class HTTPBrokerProxy : public nerikiri::CRUDBrokerProxyBase {
private:
  nerikiri::HttpClient_ptr client_;
public:
  HTTPBrokerProxy(const std::string& addr, const int64_t port) : CRUDBrokerProxyBase("HTTPBrokerProxy", "httpBrokerProxy"), 
    client_(nerikiri::client(addr, port)) {
    client_->setTimeout(0.5);
  }

  virtual ~HTTPBrokerProxy() {}

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

    virtual Value readResource(const std::string& path) override {
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


std::shared_ptr<BrokerProxyAPI> HTTPBrokerFactory::createProxy(const Value& value) {
  logger::trace("HTTPBrokerFactory::createProxy({})", value);
  if (value.hasKey("host") && value.hasKey("port")) {
      auto address = value.at("host").stringValue();
      auto port = value.at("port").intValue();
      return std::make_shared<HTTPBrokerProxy>(address, port);
  }
  logger::error("HTTPBrokerFactory::createProxy({}). Failed. Argument value does not have 'host' or 'port' value.");
  return std::make_shared<NullBrokerProxy>();
}