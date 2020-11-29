#pragma once
#include <memory>
#include "nerikiri/operation_factory.h"
#include "nerikiri/ec.h"

bool operationIsCalled = false;


auto opf1 = std::shared_ptr<nerikiri::OperationFactoryAPI>(static_cast<nerikiri::OperationFactoryAPI*>(operationFactory(
  nerikiri::Value { 
    {"typeName", "inc"},
    {"defaultArg", {
        {"arg01", 1}
      }
    }
  },
  [](auto arg) {
    operationIsCalled = true;
    return nerikiri::Value(arg.at("arg01").intValue()+1);
  }
)));

auto opf2 = std::shared_ptr<nerikiri::OperationFactoryAPI>(static_cast<nerikiri::OperationFactoryAPI*>(operationFactory(
  nerikiri::Value { 
    {"typeName", "zero"},
    {"defaultArg", {}
    }
  },
  [](auto arg)  {
    operationIsCalled = true;
    return nerikiri::Value(0);
  }
)));

auto opf3 = std::shared_ptr<nerikiri::OperationFactoryAPI>(static_cast<nerikiri::OperationFactoryAPI*>(operationFactory(
  nerikiri::Value { 
    {"typeName", "add"},
    {"defaultArg", {
        {"arg01", 0},
        {"arg02", 0}
      }
    }
  },
  [](auto arg)  {
    operationIsCalled = true;
  return nerikiri::Value(arg.at("arg01").intValue()+arg.at("arg02").intValue());
  }
)));



class OneShotEC : public nerikiri::ExecutionContextBase {
public:
    OneShotEC(const nerikiri::Value& info) : nerikiri::ExecutionContextBase("OneShotEC", nerikiri::Value::string(info.at("fullName"))) {}

public:
    virtual bool onStarted() override {
        svc();
        return true;
    }
};

auto ecf1 = std::make_shared<nerikiri::ECFactory<OneShotEC>>("oneshotECFactory");