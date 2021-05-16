#include <juiz/logger.h>

#include <juiz/operation_api.h>
#include <juiz/connection.h>
#include <juiz/process_api.h>

using namespace juiz;

class Connection : public ConnectionAPI {
private:
  const std::shared_ptr<InletAPI> inlet_;
  const std::shared_ptr<OutletAPI> outlet_;

  std::shared_ptr<OperationAPI> operation_holder_;
  //std::shared_ptr<FSMAPI> fsm_holder_;
  std::shared_ptr<Object> obj_holder_;
public:
  Connection();

  Connection(const std::string& name, const ConnectionType& type, const std::shared_ptr<InletAPI>& inlet, const std::shared_ptr<OutletAPI>& outlet) :
    ConnectionAPI("Connection", name, type), inlet_(inlet), outlet_(outlet) {}
  
  Connection(const std::string& name, const ConnectionType& type, const std::shared_ptr<InletAPI>& inlet, const std::shared_ptr<OutletAPI>& outlet, const std::shared_ptr<OperationAPI>& op) :
    ConnectionAPI("Connection", name, type), inlet_(inlet), outlet_(outlet), operation_holder_(op) {}

//  Connection(const std::string& name, const ConnectionType& type, const std::shared_ptr<OperationInletAPI>& inlet, const std::shared_ptr<OperationOutletAPI>& outlet, const std::shared_ptr<FSMAPI>& fsm) :
 //   ConnectionAPI("Connection", name, type), inlet_(inlet), outlet_(outlet), fsm_holder_(fsm) {}

  Connection(const std::string& name, const ConnectionType& type, const std::shared_ptr<InletAPI>& inlet, const std::shared_ptr<OutletAPI>& outlet, const std::shared_ptr<Object>& obj) :
    ConnectionAPI("Connection", name, type), inlet_(inlet), outlet_(outlet), obj_holder_(obj) {}


  virtual ~Connection() {
    //inlet_->finalize();
    //outlet_->finalize();
  }

public:

  virtual std::shared_ptr<InletAPI> inlet() const override { return inlet_; }

  virtual std::shared_ptr<OutletAPI> outlet() const override { return outlet_; }


  virtual Value info() const override {
    auto i = Object::info();
    i["type"] = toString(type());
    i["inlet"] = {
      {"name", inlet()->name()},
      {"ownerFullName", inlet()->ownerFullName()},
      {"fullName", inlet()->ownerFullName() + ':' + inlet()->name()}
    };
    /*
    if (obj_holder_->className() == "Operation" || obj_holder_->className() == "OperationProxy" || obj_holder_->className() == "ContainerOperation") {
      i["inlet"]["operation"] = {
        {"fullName", inlet()->ownerFullName()}
      };
    } else {
      i["inlet"]["fsm"] = {
        {"fullName", inlet()->ownerFullName()}
      };
    }
     */


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
      return inlet_->executeOwner();
    }
    return v;
  }
};

std::shared_ptr<ConnectionAPI> juiz::createConnection(const std::string& name, const ConnectionAPI::ConnectionType& type, const std::shared_ptr<InletAPI>& inlet, const std::shared_ptr<OutletAPI>& outlet) {
  logger::trace("juiz::createConnection(name={}, type={}, inlet={}, outlet={})", name, toString(type), inlet->info(), outlet->info());
  return std::make_shared<Connection>(name, type, inlet, outlet);
}

std::shared_ptr<ConnectionAPI> juiz::createConnection(const std::string& name, const ConnectionAPI::ConnectionType& type, const std::shared_ptr<InletAPI>& inlet, const std::shared_ptr<OutletAPI>& outlet, const std::shared_ptr<Object>& obj) {
  logger::trace("juiz::createConnection(name={}, type={}, inlet={}, outlet={})", name, toString(type), inlet->info(), outlet->info());
  return std::make_shared<Connection>(name, type, inlet, outlet, obj);
}

ConnectionAPI::ConnectionType juiz::connectionType(const std::string& str) {
    if (str == "PULL") return ConnectionAPI::ConnectionType::PULL;
    else if (str == "PUSH") return ConnectionAPI::ConnectionType::PUSH;
    else if (str == "EVENT") return ConnectionAPI::ConnectionType::EVENT;
    if (str == "pull") return ConnectionAPI::ConnectionType::PULL;
    else if (str == "push") return ConnectionAPI::ConnectionType::PUSH;
    else if (str == "event") return ConnectionAPI::ConnectionType::EVENT;
    return ConnectionAPI::ConnectionType::UNKNOWN;
};

std::string juiz::toString(const ConnectionAPI::ConnectionType& typ) {
    if (typ == ConnectionAPI::ConnectionType::PULL) return "PULL";
    else if (typ == ConnectionAPI::ConnectionType::PUSH) return "PUSH";
    else if (typ == ConnectionAPI::ConnectionType::EVENT) return "EVENT";
    return "UNKNOWN";
};


Value juiz::connect(const std::shared_ptr<ClientProxyAPI>& broker, const std::string& name, const std::shared_ptr<InletAPI>& inlet, const std::shared_ptr<OutletAPI>& outlet, const Value& options) {
  logger::info("juiz::connect(name={}, inlet={}, outlet={}, options={}) called", name, inlet->info(), outlet->info(), options);
  std::string defaultConnectionType = "event";
  if (options.hasKey("event")) {
    defaultConnectionType = options["event"].stringValue();
  }
  
  Value con0Info{
    {"name", name},
    {"type", "event"},
    {"broker", "CoreBroker"},
    {"inlet", inlet->info()},
    {"outlet", outlet->info()}
  };

  return broker->connection()->createConnection(con0Info);
  //return store.process()->coreBroker()->connection()->createConnection(con0Info);
}

class NullConnection : public ConnectionAPI {
private:

public:
    NullConnection() : ConnectionAPI("NullConnection", "null", ConnectionAPI::PULL) {}

    virtual ~NullConnection() {}


public:

    virtual std::shared_ptr<InletAPI> inlet() const  {
      logger::error("NullConnection::{}() called failed. Object is null", __func__);
      return nullOperationInlet();
    }

    virtual std::shared_ptr<OutletAPI> outlet() const  {
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

std::shared_ptr<ConnectionAPI> juiz::nullConnection() { return std::make_shared<NullConnection>(); }
