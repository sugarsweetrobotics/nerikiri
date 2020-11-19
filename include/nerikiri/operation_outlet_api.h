#pragma once 

#include <nerikiri/value.h>
#include <nerikiri/connection_api.h>


namespace nerikiri {

  class OperationAPI;
  class ConnectionAPI;
    
  class OperationOutletAPI {
  public:
    virtual ~OperationOutletAPI() {}

  public:
    /*
    virtual Value put(Value&& v) = 0;

    virtual Value invoke() = 0;
    */

    virtual OperationAPI* owner() = 0;

    virtual Value get() const = 0;

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const = 0;

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& c) = 0;
    
    virtual Value removeConnection(const std::string& _fullName) = 0;

    virtual Value info() const = 0;
  };

  std::shared_ptr<OperationOutletAPI> nullOperationOutlet();
}