#pragma once 

//#include <nerikiri/connection_api.h>

#include <nerikiri/logger.h>
#include <nerikiri/operation_api.h>

namespace nerikiri {

  class ConnectionAPI;
  class OperationAPI;

  class OperationInletAPI {
  public:
    virtual ~OperationInletAPI() {}
  public:
    virtual std::string name() const = 0;

    virtual Value defaultValue() const = 0;
    
    virtual Value get() const = 0;

    virtual Value put(const Value& value) = 0;

    virtual OperationAPI* owner() = 0;

    virtual Value info() const = 0;

    virtual bool isUpdated() const = 0;

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const = 0;

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) = 0;
    
    virtual Value removeConnection(const std::string& _fullName) = 0;
  };

  std::shared_ptr<OperationInletAPI> nullOperationInlet();
}