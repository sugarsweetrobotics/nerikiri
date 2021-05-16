#pragma once

#include <juiz/broker_api.h>

/**
 * 
 * 
 */
class HTTPBrokerFactory : public juiz::BrokerFactoryAPI {
public:

  HTTPBrokerFactory(const std::string& fullName): BrokerFactoryAPI ("HTTPBrokerFactory", "HTTPBroker", fullName) {}
  virtual ~HTTPBrokerFactory() {}

  //virtual std::string brokerTypeFullName() const override { return "HTTPBroker"; }

public:
  virtual std::shared_ptr<juiz::BrokerAPI> create(const juiz::Value& value) override;
  
  virtual std::shared_ptr<juiz::ClientProxyAPI> createProxy(const juiz::Value& value) override;

};