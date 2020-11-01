#include <iostream>

#include <nerikiri/logger.h>
#include <nerikiri/functional.h>
#include <nerikiri/operation_base.h>

using namespace nerikiri;

Value OperationOutletBase::put(Value&& v) {
  outputBuffer_.push(v);
  for (auto& c : connections_.connections()) {
    c->put(v);
  }
  return std::forward<Value>(v);
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

OperationBase::OperationBase(const std::string& typeName, const std::string& operationTypeName, const std::string& fullName, const Value& defaultArgs /*= {}*/):
  OperationAPI(typeName, fullName), operationTypeName_(operationTypeName), outlet_(std::make_shared<OperationOutletBase>(this)) {
  defaultArgs.const_object_for_each([this](const std::string& key, const Value& value) {
    inlets_.emplace_back(std::make_shared<OperationInletBase>(key, this, value));
  });
}

OperationBase::~OperationBase() {}

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
