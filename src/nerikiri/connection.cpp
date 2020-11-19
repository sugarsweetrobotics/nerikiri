#include "nerikiri/logger.h"

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

    virtual std::shared_ptr<OperationInletAPI> inlet() const override { return inlet_; }

    virtual std::shared_ptr<OperationOutletAPI> outlet() const override { return outlet_; }

    virtual Value pull() override;

    virtual Value put(const Value& value) override;
};

std::shared_ptr<ConnectionAPI> nerikiri::createConnection(const std::string& name, const ConnectionAPI::ConnectionType& type, const std::shared_ptr<OperationInletAPI>& inlet, const std::shared_ptr<OperationOutletAPI>& outlet) {
  return std::make_shared<Connection>(name, type, inlet, outlet);
}


Value Connection::pull() {
  if (this->isPull()) {
    return outlet_->owner()->invoke();
  }
  return outlet_->get();
}

Value Connection::put(const Value& value) {
  auto v = inlet_->put(value);
  if (this->isPush() || this->isEvent()) {
    return inlet_->owner()->execute();
  }
  return v;
}


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