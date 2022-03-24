#include <sstream>

#include <juiz/connection.h>
#include "operation_outlet_base.h"

using namespace juiz;
/////

/**
 * inletOwnerClassName must be "operation" or "fsm"
 */
static bool check_the_same_route_connection_exists(const std::vector<std::shared_ptr<ConnectionAPI>>& connections, const Value& conInfo, const std::string& inletOwnerClassName) {
    auto flag = false;
    juiz::functional::for_each<std::shared_ptr<ConnectionAPI>>(connections, [&flag, &conInfo, &inletOwnerClassName](auto con) {
      // もし接続の所有しているinletのオーナー名と接続情報の名前が一致していたら
        if ((con->inlet()->ownerFullName() == Value::string(conInfo.at("inlet").at(inletOwnerClassName).at("fullName"))) && 
            (con->inlet()->name() != Value::string(conInfo.at("inlet").at("name"))) &&
            (con->outlet()->ownerFullName() == Value::string(conInfo.at("outlet").at("operation").at("fullName")))) {
          flag = true;
        }
    });
    return flag;
}

static bool check_the_same_name_connection_exists(const std::vector<std::shared_ptr<ConnectionAPI>>& connections, const std::string& name) {
    auto con = juiz::functional::find<std::shared_ptr<ConnectionAPI>>(connections, [&name](auto c) {
        return c->fullName() == name;
    });
    if (con) return true;
    return false;
}

static std::string applyConnectionAutoRename(const std::string& name, const int count_hint=0) {
  std::stringstream ss;
  ss << name << count_hint;
  return ss.str();
}


/////

Value OperationOutletBase::put(Value&& v) {
  logger::trace2_object to("OperaitonOutletBase({})::put({}) called", this->ownerFullName(), v);
  outputBuffer_.push(v);
  for (auto& c : connections_.connections()) {
    logger::verbose(" - Connection({}) is detected. Putting to the Connection", c->fullName());
    c->put(v);
  }
  return std::forward<Value>(v);
}

Value OperationOutletBase::connectTo(const std::shared_ptr<InletAPI>& inlet, const Value& connectionInfo_) {
  logger::trace2_object to("OperationOutletBase::connectTo(inlet(info={}), connectionInfo={}) called", inlet->info(), connectionInfo_);
  auto con = createConnection(Value::string(connectionInfo_["name"]), connectionType(Value::string(connectionInfo_["type"])), inlet, operation_->outlet(), nullptr);
  if (con->isNull()) {
    return Value::error(logger::error("OperationBase::addConnection() failed. Passing connection is null"));
  }
  auto v = connections_.addConnection(con);
  if (v.isError()) {
    logger::error("OperationOutletBase::addConnection failed: {}", v.getErrorMessage());
    return v;
  }
  return v;
}
    
Value OperationOutletBase::disconnectFrom(const std::shared_ptr<InletAPI>& inlet) {
  // TODO: 未実装
  return Value::error(logger::error("OperationOutletBase::disconnectFrom() called. but not implemented"));
}
