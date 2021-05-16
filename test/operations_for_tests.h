#pragma once
#include <memory>
#include <juiz/operation.h>
#include <juiz/ec.h>
#include <juiz/container.h>

bool operationIsCalled = false;

bool operationIncIsCalled = false;
bool operationZeroIsCalled = false;
bool operationAddIsCalled = false;


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
    operationIncIsCalled = true;
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
    operationZeroIsCalled = true;
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
    operationAddIsCalled = true;
  return nerikiri::Value(arg.at("arg01").intValue()+arg.at("arg02").intValue());
  }
)));

auto opf4 = std::shared_ptr<nerikiri::OperationFactoryAPI>(static_cast<nerikiri::OperationFactoryAPI*>(operationFactory(
  nerikiri::Value { 
    {"typeName", "one"},
    {"defaultArg", {}
    }
  },
  [](auto arg)  {
    operationIsCalled = true;
    return nerikiri::Value(1);
  }
)));

class OneShotEC : public nerikiri::ExecutionContextBase {
public:
    OneShotEC(const nerikiri::Value& info) : nerikiri::ExecutionContextBase("OneShotEC") {}

public:
    virtual bool onStarted() override {
        svc();
        return true;
    }
};

auto ecf1 = std::make_shared<nerikiri::ECFactory<OneShotEC>>();

struct MyStruct {
  int32_t value;
  MyStruct(): value(0) {}
};

auto cf0 = std::make_shared<nerikiri::ContainerFactory<MyStruct>>();


auto copf0 = std::shared_ptr<nerikiri::ContainerOperationFactoryAPI>(static_cast<nerikiri::ContainerOperationFactoryAPI*>(nerikiri::containerOperationFactory<MyStruct>(
  nerikiri::Value { 
    {"typeName", {"set"}},
    {"defaultArg", {
      {"value", {0}}
    }}
  },
  [](auto& container, auto arg)  {
    if (arg["value"].isIntValue()) {
      container.value = arg["value"].intValue();
    }
    return arg;
  }
)));

auto copf2 = std::shared_ptr<nerikiri::ContainerOperationFactoryAPI>(static_cast<nerikiri::ContainerOperationFactoryAPI*>(nerikiri::containerOperationFactory<MyStruct>(
  nerikiri::Value { 
    {"typeName", {"get"}},
    {"defaultArg", {
      {}
    }}
  },
  [](auto& container, auto arg)  {
    
    return container.value;
  }
)));



auto copf1 = std::shared_ptr<nerikiri::ContainerOperationFactoryAPI>(static_cast<nerikiri::ContainerOperationFactoryAPI*>(nerikiri::containerOperationFactory<MyStruct>(
  nerikiri::Value { 
    {"typeName", {"inc"}},
    {"defaultArg", {
      {}
    }}
  },
  [](auto& container, auto arg)  {
    container.value++;
    return arg;
  }
)));
