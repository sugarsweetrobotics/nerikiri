#include <iostream>

#include <nerikiri/logger.h>
#include <nerikiri/functional.h>
#include <nerikiri/operation_base.h>

using namespace nerikiri;

namespace nerikiri {
class OperationOutletBase  : public OperationOutletAPI {
  public:
    OperationOutletBase(OperationAPI* operation): operation_(operation) {}
    virtual ~OperationOutletBase() {}

  public:
    OperationAPI* operation_;

    NewestValueBuffer outputBuffer_;
    ConnectionContainer connections_;
  public:
    virtual OperationAPI* owner() override { return operation_; }
  

    virtual Value get() const override { return outputBuffer_.pop(); }

    virtual Value invoke() { 
      outputBuffer_.push(operation_->invoke());
      return outputBuffer_.pop(); 
    }

    virtual Value info() const override {
        return {
            {"connections", {
                nerikiri::functional::map<Value, std::shared_ptr<ConnectionAPI>>(connections_.connections(), [](auto c) { return c->info(); })
            }}
        };
    }

    virtual Value put(Value&& v);

    virtual std::vector<std::shared_ptr<ConnectionAPI>> connections() const override { return connections_.connections(); }

    virtual Value addConnection(const std::shared_ptr<ConnectionAPI>& con) override {
      if (con->isNull()) {
        return Value::error(logger::error("OperationBase::addConnection() failed. Passing connection is null"));
      }
      return connections_.addConnection(con);
    }
    
    virtual Value removeConnection(const std::string& _fullName) override {
      return connections_.removeConnection(_fullName);
    }
  };
}



Value OperationOutletBase::put(Value&& v) {
  outputBuffer_.push(v);
  for (auto& c : connections_.connections()) {
    c->put(v);
  }
  return std::forward<Value>(v);
}

OperationInletBase::OperationInletBase(const std::string& name, OperationAPI* operation, const Value& defaultValue) 
: name_(name), operation_(operation), default_(defaultValue), argument_updated_(false), 
     buffer_(std::make_shared<NewestValueBuffer>(defaultValue))
{
  
}
Value OperationInletBase::info() const {
  return {
      {"name", name()},
      {"value", get()},
      {"defaultValue", defaultValue()},
      {"connections", {
          nerikiri::functional::map<Value, std::shared_ptr<ConnectionAPI>>(connections_.connections(), [](auto c) { return c->info(); })
      }}
  };
}

Value OperationInletBase::collectValues() {
  for (auto& con : connections_.connections()) {
    if (con->isPull()) { 
      logger::trace(" - Pulling by connection: {}", con->fullName());
      auto v = put(con->pull());
      if (v.isError()) {
        logger::warn(" - Pulling by connection {} failed. Error message is : {}", con->fullName(), v.getErrorMessage());
        continue;
      }
      break; // 今はどれか一つでもpullできたらそれで終了という仕様．いいのかこれで？
    }
  }
  return buffer_->pop();
}

Value OperationInletBase::put(const Value& value) {
  std::lock_guard<std::mutex> lock(argument_mutex_);
  if (value.isError()) {
    logger::error("OperationInletBase::{} failed. Argument is error({})", __func__, value.getErrorMessage());
    return value;
  }
  buffer_->push(value);
  argument_updated_ = true;
  return value;
}
/**
 * コネクションに対してデータの要求を行っていく．
 * リスト内のpull型の最初のコネクションのデータのみを使う．それ以降はpull型であってもexecuteしない
 * どれもpull型でない，もしくは全てのpullが失敗する場合はデフォルト値を返す
 * 議論：　pull型なら等しくexecuteして，その上で捨てた方が良いんじゃないか
 * 
 *
Value OperationInputBase::collectValues() {
    std::lock_guard<std::mutex> lock(argument_mutex_);
    return nerikiri::functional::map<std::pair<std::string, Value>, std::string, OperationInlet>(inlets_, [](auto key, auto value) {
        return std::pair<std::string, Value>{key, value.collectValues()};
    });
}
 */


//OperationBase::OperationBase(const OperationBase& op): OperationAPI() {}

OperationBase::OperationBase(const std::string& className, const std::string& typeName, const std::string& fullName, const Value& defaultArgs /*= {}*/):
  OperationAPI(className, typeName, fullName), outlet_(std::make_shared<OperationOutletBase>(this)) {
  defaultArgs.const_object_for_each([this](const std::string& key, const Value& value) {
    inlets_.emplace_back(std::make_shared<OperationInletBase>(key, this, value));
  });
}

OperationBase::~OperationBase() {}

std::shared_ptr<OperationOutletAPI> OperationBase::outlet() const  { return outlet_; }

Value OperationBase::fullInfo() const {
  auto i = info();
  i["outlet"] = outlet()->info();
  i["inlets"] = nerikiri::functional::map<Value, std::shared_ptr<OperationInletAPI>>(inlets(), [](auto inlet) { return inlet->info(); });
  return i;
}
    


Value OperationBase::invoke() {
  try {
    if (nerikiri::functional::for_all<std::shared_ptr<OperationInletBase>>(inlets_, [](auto inlet) { return inlet->isUpdated(); })) {
      return call(
        nerikiri::functional::map<std::pair<std::string, Value>, std::shared_ptr<OperationInletBase>>(inlets_, [](auto inlet) {
          return std::pair<std::string, Value>{inlet->name(), inlet->collectValues()};
      }));
    } else {
      return outlet_->get();
    }
  } catch (const std::exception& ex) {
    return Value::error(logger::error("OperationBase({})::invoke() failed. Exception occurred {}", fullName(), std::string(ex.what())));
  }
}

Value OperationBase::execute() {
  try {
    return outlet_->put(invoke());
  } catch (const std::exception& ex) {
    return Value::error(logger::error("OperationBase({})::invoke() failed. Exception occurred {}", fullName(), std::string(ex.what())));
  }
}
