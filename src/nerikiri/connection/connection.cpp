#include "nerikiri/logger.h"

#include <nerikiri/operation_api.h>
#include "nerikiri/connection.h"


using namespace nerikiri;

class Connection : public ConnectionAPI {
private:
  const std::shared_ptr<OperationInletAPI> inlet_;

  const std::shared_ptr<OperationOutletAPI> outlet_;
public:
  Connection();

  Connection(const std::string& name, const ConnectionType& type, const std::shared_ptr<OperationInletAPI>& inlet, const std::shared_ptr<OperationOutletAPI>& outlet) :
    ConnectionAPI("Connection", name, type), inlet_(inlet), outlet_(outlet) {}
  
  virtual ~Connection() {}

public:

  virtual std::shared_ptr<OperationInletAPI> inlet() const override { return inlet_; }

  virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return outlet_; }


  virtual Value info() const override {
    auto i = Object::info();
    i["type"] = toString(type());
    i["inlet"] = {
      {"name", inlet()->name()},
      {"ownerFullName", inlet()->ownerFullName()},
      {"fullName", inlet()->ownerFullName() + ':' + inlet()->name()}
    };
    i["outlet"] = {
      {"ownerFullName", outlet()->ownerFullName()}
    };
    // i["outlet"] = outlet()->info();
    return i;
  }

  virtual Value pull() override {
    if (this->isPull()) {
      //return outlet_->owner()->invoke();
      return outlet_->invokeOwner();
    }
    return outlet_->get();
  }


  virtual Value put(const Value& value) override {
    logger::trace("Connection({})::put({}) called", fullName(), value);
    auto v = inlet_->put(value);
    if (this->isEvent()) {
      logger::trace(" - executing owner operation({})", inlet_->ownerFullName());
      return inlet_->executeOwner(); //owner()->execute();
    }
    return v;
  }
};

std::shared_ptr<ConnectionAPI> nerikiri::createConnection(const std::string& name, const ConnectionAPI::ConnectionType& type, const std::shared_ptr<OperationInletAPI>& inlet, const std::shared_ptr<OperationOutletAPI>& outlet) {
  logger::trace("nerikiri::createConnection(name={}, type={}, inlet={}, outlet={})", name, toString(type), inlet->info(), outlet->info());
  return std::make_shared<Connection>(name, type, inlet, outlet);
}

ConnectionAPI::ConnectionType nerikiri::connectionType(const std::string& str) {
    if (str == "PULL") return ConnectionAPI::ConnectionType::PULL;
    else if (str == "PUSH") return ConnectionAPI::ConnectionType::PUSH;
    else if (str == "EVENT") return ConnectionAPI::ConnectionType::EVENT;
    if (str == "pull") return ConnectionAPI::ConnectionType::PULL;
    else if (str == "push") return ConnectionAPI::ConnectionType::PUSH;
    else if (str == "event") return ConnectionAPI::ConnectionType::EVENT;
    return ConnectionAPI::ConnectionType::UNKNOWN;
};

std::string nerikiri::toString(const ConnectionAPI::ConnectionType& typ) {
    if (typ == ConnectionAPI::ConnectionType::PULL) return "PULL";
    else if (typ == ConnectionAPI::ConnectionType::PUSH) return "PUSH";
    else if (typ == ConnectionAPI::ConnectionType::EVENT) return "EVENT";
    return "UNKNOWN";
};





class NullConnection : public ConnectionAPI {
private:

public:
    NullConnection() : ConnectionAPI("NullConnection", "null", ConnectionAPI::PULL) {}

    virtual ~NullConnection() {}


public:

    virtual std::shared_ptr<OperationInletAPI> inlet() const  {
      logger::error("NullConnection::{}() called failed. Object is null", __func__);
      return nullOperationInlet();
    }

    virtual std::shared_ptr<OperationOutletAPI> outlet() const  {
      logger::error("NullConnection::{}() called failed. Object is null", __func__);
      return nullOperationOutlet();
    }

    virtual Value pull() {
      return Value::error(logger::error("NullConnection::{}() called failed. Object is null", __func__));
    }

    virtual Value put(const Value& value) {
      return Value::error(logger::error("NullConnection::{}() called failed. Object is null", __func__));
    }

};

std::shared_ptr<ConnectionAPI> nerikiri::nullConnection() { return std::make_shared<NullConnection>(); }