#include "operation_inlet_base.h"
#include <nerikiri/connection.h>


using namespace nerikiri;



OperationInletBase::OperationInletBase(const std::string& name, OperationAPI* operation, const Value& defaultValue) 
: name_(name), operation_(operation), default_(defaultValue), argument_updated_(false), 
     buffer_(std::make_shared<NewestValueBuffer>(defaultValue))
{
  
}


Value OperationInletBase::info() const {
  return {
    {"name", name()},
      {"ownerFullName", operation_->fullName()},
    {"value", get()},
    {"defaultValue", defaultValue()},
    {"connections", {
      nerikiri::functional::map<Value, std::shared_ptr<ConnectionAPI>>(connections_.connections(), [](auto c) {
        return c->info();
      })
    }}
  };
}

Value OperationInletBase::collectValues() {
  logger::trace("OperationInletBase::collectValues() called");
  for (auto& con : connections_.connections()) {
    if (con->isPull()) { 
      logger::trace(" - Pulling by connection: {}", con->fullName());
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
  logger::trace("OperaitonInletBase::put({}) called", value);
  std::lock_guard<std::mutex> lock(argument_mutex_);
  if (value.isError()) {
    logger::error("OperationInletBase::{} failed. Argument is error({})", __func__, value.getErrorMessage());
    return value;
  }
  logger::trace(" - pushing the value to the buffer");
  buffer_->push(value);
  argument_updated_ = true;
  return value;
}



Value OperationInletBase::connectTo(const std::shared_ptr<OperationOutletAPI>& outlet, const Value& connectionInfo) {  
  return addConnection( createConnection(Value::string(connectionInfo["name"]), connectionType(Value::string(connectionInfo.at("type"))), operation_->inlet(this->name()), outlet, nullptr));
}
    
Value OperationInletBase::disconnectFrom(const std::shared_ptr<OperationOutletAPI>& outlet) {
  
}
