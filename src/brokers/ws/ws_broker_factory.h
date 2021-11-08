#pragma once

#include <juiz/broker_api.h>

#include "ws.h"

/**
 * 
 * 
 */
class WebSocketBrokerFactory : public juiz::BrokerFactoryAPI {
public:
  WebSocketBrokerFactory(const std::string& fullName): BrokerFactoryAPI ("WSBrokerFactory", "WSBroker", fullName) {}
  virtual ~WebSocketBrokerFactory() {}

public:
  virtual std::shared_ptr<juiz::BrokerAPI> create(const juiz::Value& value) override;
  
  virtual std::shared_ptr<juiz::ClientProxyAPI> createProxy(const juiz::Value& value) override;

};