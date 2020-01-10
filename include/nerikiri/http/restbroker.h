#pragma once

#include "nerikiri/broker.h"

namespace nerikiri::http {

  class RESTBroker : public Broker {


  public:
    RESTBroker();
    virtual ~RESTBroker();

  };

};
