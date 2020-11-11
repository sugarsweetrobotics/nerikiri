#pragma once

#include <nerikiri/broker_factory_api.h>

/**
 * 
 * 
 */
class HTTPBrokerFactory : public nerikiri::BrokerFactoryAPI {
public:

  HTTPBrokerFactory(): BrokerFactoryAPI ("HTTPBrokerFactory", "httpBroker") {}
  virtual ~HTTPBrokerFactory() {}

  virtual std::string brokerTypeFullName() const override { return "HTTPBroker"; }

public:
  virtual std::shared_ptr<nerikiri::BrokerAPI> create(const nerikiri::Value& value) override;
  
  virtual std::shared_ptr<nerikiri::BrokerProxyAPI> createProxy(const nerikiri::Value& value) override;

};