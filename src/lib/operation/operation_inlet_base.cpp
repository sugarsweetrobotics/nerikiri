#include "operation_inlet_base.h"
#include <juiz/connection.h>


using namespace juiz;



OperationInletBase::OperationInletBase(const std::string& name, OperationAPI* operation, const Value& defaultValue) 
: name_(name), operation_(operation), default_(defaultValue), argument_updated_(false), 
     buffer_(std::make_shared<NewestValueBuffer>(defaultValue))
{
  
}

Value OperationInletBase::fullInfo() const {
  logger::trace3_object to("OperationInletBase({})::fullInfo() called", fullPath());
  return {
    {"name", name()},
    {"ownerFullName", operation_->fullName()},
    {"value", get()},
    {"defaultValue", defaultValue()},
    {"connections", {
      juiz::functional::map<Value, std::shared_ptr<ConnectionAPI>>(connections_.connections(), [](auto c) {
        return c->info();
      })
    }}//,
    //{"operation", operation_->info()}
  };
}

Value OperationInletBase::info() const {
  return {
    {"name", name()},
    {"ownerFullName", operation_->fullName()},
    {"operation", operation_->info()}
  };
}

Value OperationInletBase::collectValues() {
  logger::trace3_object to("OperationInletBase::collectValues({}:{}) called", ownerFullName(), name());
  for (auto& con : connections_.connections()) {
    if (con->isPull()) {
      logger::verbose(" - Pulling by connection: {}", con->fullName());
      auto v = put(con->pull());
      if (v.isError()) {
        logger::warn("OperationInletBase::collectValues(): Pulling by connection {} failed. Error message is : {}", con->fullName(), v.getErrorMessage());
        continue;
      }
      break; // 今はどれか一つでもpullできたらそれで終了という仕様．いいのかこれで？
    }
  }
  return buffer_->pop();
}

Value OperationInletBase::put(const Value& value) {  
  logger::trace3_object to("OperaitonInletBase({})::put(value) called", ownerFullName() + ":" + name());
  std::lock_guard<std::mutex> lock(argument_mutex_);
  if (value.isError()) {
    logger::error("OperationInletBase({})::{} failed. Argument is error({})", ownerFullName() + ":" + name(), __func__, value.getErrorMessage());
    return value;
  }
  logger::verbose(" - pushing the value to the buffer");
  buffer_->push(value);
  argument_updated_ = true;
  return value;
}



Value OperationInletBase::connectTo(const std::shared_ptr<OutletAPI>& outlet, const Value& connectionInfo) {  
  logger::trace3_object to("OperationInletBase::connectTo(outlet(info={}), connectionInfo={}) called", outlet->info(), connectionInfo);
  auto con = createConnection(Value::string(connectionInfo["name"]), connectionType(Value::string(connectionInfo.at("type"))), operation_->inlet(this->name()), outlet, nullptr);
  auto v = connections_.addConnection(con);
  if (v.isError()) {
    logger::error("OperationInletBase::addConnection failed: {}", v.getErrorMessage());
    return v;
  }
  return v;
}
    
Value OperationInletBase::disconnectFrom(const std::shared_ptr<OutletAPI>& outlet) {
  
}


void OperationInletBase::finalize() {
  auto conns = connections_.connections();
  std::for_each(conns.begin(), conns.end(), [this](auto c) {

  });
  connections_.clear();
}