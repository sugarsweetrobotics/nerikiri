#include <iostream>

#include <juiz/logger.h>
#include <juiz/utils/functional.h>
#include "operation_base.h"
#include "operation_inlet_base.h"
#include "operation_outlet_base.h"
#include "argument_inlet.h"

using namespace juiz;



/**
 * コネクションに対してデータの要求を行っていく．
 * リスト内のpull型の最初のコネクションのデータのみを使う．それ以降はpull型であってもexecuteしない
 * どれもpull型でない，もしくは全てのpullが失敗する場合はデフォルト値を返す
 * 議論：　pull型なら等しくexecuteして，その上で捨てた方が良いんじゃないか
 * 
 *
Value OperationInputBase::collectValues() {
    std::lock_guard<std::mutex> lock(argument_mutex_);
    return juiz::functional::map<std::pair<std::string, Value>, std::string, OperationInlet>(inlets_, [](auto key, auto value) {
        return std::pair<std::string, Value>{key, value.collectValues()};
    });
}
 */


//OperationBase::OperationBase(const OperationBase& op): OperationAPI() {}

OperationBase::OperationBase(const std::string& className, const std::string& typeName, const std::string& fullName, const Value& defaultArgs /*= {}*/):
  OperationAPI(className, typeName, fullName), outlet_(std::make_shared<OperationOutletBase>(this)), _first_output_done(false) {
  event_inlet_ = std::make_shared<OperationInletBase>("__event__", this, Value({}));
  argument_inlet_ = std::make_shared<ArgumentInlet>("__argument__", this, Value({}));
  defaultArgs.const_object_for_each([this](const std::string& key, const Value& value) {
    inlets_.emplace_back(std::make_shared<OperationInletBase>(key, this, value));
  });
  
}

OperationBase::~OperationBase() {}


void OperationBase::finalize() {
  logger::trace2_object to("OperationBase(fullName={})::finalize() called", fullName());
  std::for_each(inlets_.begin(), inlets_.end(), [this](auto inlet) {
    inlet->finalize();
  });
  inlets_.clear();
}

std::shared_ptr<OutletAPI> OperationBase::outlet() const  { return outlet_; }

Value OperationBase::fullInfo() const {
  auto i = info();
  i["outlet"] = outlet()->fullInfo();
  i["inlets"] = juiz::functional::map<Value, std::shared_ptr<InletAPI>>(inlets(), [](auto inlet) { return inlet->fullInfo(); });
  return i;
}

Value OperationBase::info() const {
  auto i = Object::info();
  i["broker"] = {
    {"typeName", "CoreBroker"}
  };
//  i["outlet"] = outlet()->info();
//  i["inlets"] = juiz::functional::map<Value, std::shared_ptr<OperationInletAPI>>(inlets(), [](auto inlet) { return inlet->info(); });
  return i;
}

std::shared_ptr<InletAPI> OperationBase::inlet(const std::string& name) const {
  if (name == "__event__")  return event_inlet_;
  if (name == "__argument__")  return argument_inlet_;
  auto i = juiz::functional::find<std::shared_ptr<InletAPI>>(inlets(), [&name](auto i) { return i->name() == name; });
  if (i) return i.value();
  logger::error("OperationBase(typeName={}, fullName={})::inlet(name={}) requested. But argument {} not found.", typeName(), fullName(), name, name);
  return nullOperationInlet();
}

std::vector<std::shared_ptr<InletAPI>> OperationBase::inlets() const { 
  return {inlets_.begin(), inlets_.end()};
}

namespace {
  bool has_pull_connection(const std::shared_ptr<InletAPI>& inlet) {
    auto cons = inlet->connections();
    bool flag = false;
    std::for_each(cons.begin(), cons.end(), [&flag](auto con) {
      flag |= con->isPull();
    });
    return flag;
  }
}
/**
 * invoke　
 */
Value OperationBase::invoke() {
  logger::trace2_object to("OperationBase({})::invoke() called", fullName());
  try {
    /// もし，一つでもインレット（引数）がアップデートされていたらcollectValuesでデータを集めてcallします．
    auto is_inlet_updated = [this]() { return juiz::functional::for_any<std::shared_ptr<OperationInletBase>>(inlets_, [](auto inlet) { return inlet->isUpdated(); }); };
    auto do_have_pull_connection = [this]() { return juiz::functional::for_any<std::shared_ptr<OperationInletBase>>(inlets_, [](auto inlet) { return has_pull_connection(inlet); }); };
    if (!_first_output_done || is_inlet_updated() || do_have_pull_connection()) {
      logger::verbose("OperationBase({})::invoke() inlet updated. Collecting data and calling...", fullName());
      Value&& v = call(
        juiz::functional::map<std::pair<std::string, Value>, std::shared_ptr<OperationInletBase>>(inlets_, [](auto inlet) {
          return std::pair<std::string, Value>{inlet->name(), inlet->collectValues()};
      }));
      _first_output_done = true;

      logger::verbose("OperationBase({})::invoke() exit", fullName());
      return v;
    } else {
      logger::verbose("OperationBase({})::invoke() not updated. Just output the outlet buffer.", fullName());
      return outlet_->get();
    }
  } catch (std::exception& ex) {
    return Value::error(logger::error("OperationBase({})::invoke() failed. Exception occurred {}", fullName(), std::string(ex.what())));
  }
  
}

Value OperationBase::execute() {
  logger::trace2_object to("OperationBase({})::execute() called", fullName());
  try {
    auto&& v = outlet_->put(invoke());
    logger::verbose("OperationBase({})::execute() exit", fullName());
    return v;
  } catch (std::exception& ex) {
    return Value::error(logger::error("OperationBase({})::execute() failed. Exception occurred {}", fullName(), std::string(ex.what())));
  }
}
