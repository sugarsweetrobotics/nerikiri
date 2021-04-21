#include <iostream>

#include <nerikiri/logger.h>
#include <nerikiri/functional.h>
#include "operation_base.h"
#include "operation_inlet_base.h"
#include "operation_outlet_base.h"

using namespace nerikiri;



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
  event_inlet_ = std::make_shared<OperationInletBase>("__event__", this, Value({}));
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

Value OperationBase::info() const {
  auto i = Object::info();
  i["broker"] = {
    {"typeName", "CoreBroker"}
  };
//  i["outlet"] = outlet()->info();
//  i["inlets"] = nerikiri::functional::map<Value, std::shared_ptr<OperationInletAPI>>(inlets(), [](auto inlet) { return inlet->info(); });
  return i;
}

std::shared_ptr<OperationInletAPI> OperationBase::inlet(const std::string& name) const {
  if (name == "__event__")  return event_inlet_;
  auto i = nerikiri::functional::find<std::shared_ptr<OperationInletAPI>>(inlets(), [&name](auto i) { return i->name() == name; });
  if (i) return i.value();
  logger::error("OperationBase(typeName={}, fullName={})::inlet(name={}) requested. But argument {} not found.", typeName(), fullName(), name, name);
  return nullOperationInlet();
}

std::vector<std::shared_ptr<OperationInletAPI>> OperationBase::inlets() const { 
  return {inlets_.begin(), inlets_.end()};
}


Value OperationBase::invoke() {
  logger::trace("OperationBase({})::invoke() called", fullName());
  try {
    if (nerikiri::functional::for_all<std::shared_ptr<OperationInletBase>>(inlets_, [](auto inlet) { return inlet->isUpdated(); })) {
      logger::trace("OperationBase({})::invoke() inlet updated. Collecting data and calling...", fullName());
      return call(
        nerikiri::functional::map<std::pair<std::string, Value>, std::shared_ptr<OperationInletBase>>(inlets_, [](auto inlet) {
          return std::pair<std::string, Value>{inlet->name(), inlet->collectValues()};
      }));
    } else {
      logger::trace("OperationBase({})::invoke() not updated. Just output the outlet buffer.", fullName());
      return outlet_->get();
    }
  } catch (const std::exception& ex) {
    return Value::error(logger::error("OperationBase({})::invoke() failed. Exception occurred {}", fullName(), std::string(ex.what())));
  }
  
}

Value OperationBase::execute() {
  logger::trace("OperationBase({})::execute() called", fullName());
  try {
    return outlet_->put(invoke());
  } catch (const std::exception& ex) {
    return Value::error(logger::error("OperationBase({})::invoke() failed. Exception occurred {}", fullName(), std::string(ex.what())));
  }
}
