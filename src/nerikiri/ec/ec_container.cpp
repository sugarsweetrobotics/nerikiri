#include <iostream>
#include <algorithm>
#include <nerikiri/functional.h>
#include <nerikiri/process_store.h>
#include <nerikiri/container.h>
#include <nerikiri/logger.h>
#include "nerikiri/ec/ec_container.h"
#include <nerikiri/ec.h>

using namespace nerikiri;

struct _ECContainerStruct {
    std::string fullName;
    std::string typeName;
    std::shared_ptr<ExecutionContextAPI> ec;
    std::string currentState;
    const std::vector<std::string> availableStates;
    _ECContainerStruct(): currentState(""), availableStates({"started", "stopped"}) {}
};


bool 
nerikiri::setupECContainer(nerikiri::ProcessStore& store) {
  store.add<ContainerFactoryAPI>(std::shared_ptr<ContainerFactoryAPI>(static_cast<ContainerFactoryAPI*>(containerFactory<_ECContainerStruct>({
	    {"className", "ECContainer"}
	  }))));
    
    store.add<ContainerOperationFactoryAPI>(std::shared_ptr<ContainerOperationFactoryAPI>(static_cast<ContainerOperationFactoryAPI*>(containerOperationFactory<_ECContainerStruct>(
        {
          {"typeName", "activate_state"},
          {"defaultArg", {
              {"stateName", "__invalid__"}
          }},
        },
        [](auto& container, auto arg) {
            const auto s = Value::string(arg["stateName"]);
            const auto c = container.currentState;
            if (std::find(container.availableStates.begin(), container.availableStates.end(), s) == container.availableStates.end()) {
                return Value::error(logger::error("[ECStateContainer] failed to activate {} in FSM ({}). This is not found in available state list.", s, container.fullName));
            }
            logger::trace("ECContainerStruct::activate_state_{} succeeded", s);
            container.currentState = s;
            return arg;
        }))
    ));

    store.add<ContainerOperationFactoryAPI>(std::shared_ptr<ContainerOperationFactoryAPI>(static_cast<ContainerOperationFactoryAPI*>(containerOperationFactory<_ECContainerStruct>(
        {
          {"typeName", "get_state"},
          {"defaultArg", {
          }},
        },
        [](auto& container, auto arg) {
            return container.currentState;
        }))
    ));

    return true;
}

Value nerikiri::createEC(ProcessStore& store, const std::string& fullName, const Value& ecInfo) {
    logger::info("nerikiri::createEC({})", ecInfo);
    //auto fullName = loadFullName(store.fsms(), fsmInfo);
    auto container = store.get<ContainerFactoryAPI>("_ECContainerStruct")->create(fullName);
    if (container->isNull()) {
        return Value::error(logger::error("ObjectFactory::createEC failed. NullContainer is returned from factory."));
    }
    auto c = std::dynamic_pointer_cast<Container<_ECContainerStruct>>(container);
    if (!c) {
        return Value::error(logger::error("ObjectFactory::createEC failed. Failed to cast to Container<_ECContainerStruct>."));
    }

    auto defaultStateName = "stopped";

    std::cout << "Execution" << " c" << std::endl;
    c->ptr()->fullName = fullName;
    c->setClassName("ExecutionContext");
    c->setTypeName(ecInfo["typeName"].stringValue());
    c->ptr()->currentState = defaultStateName;
    /// ここでECの本体を設定する
    c->ptr()->ec = store.executionContextFactory(Value::string(ecInfo.at("typeName")))->create(ecInfo);
    //if (c->ptr()->ec->isNull()) {
    //    return Value::error(logger::error("ObjectFactory::createEC failed. Failed to create ExecutionContext(typeName={})", Value::string(ecInfo.at("typeName"))));
    //}
    auto getter = store.get<ContainerOperationFactoryAPI>("_ECContainerStruct:get_state")->create<OperationAPI>(container->fullName() + ":" + "get_state.ope");
    getter->setOwner(container);

    auto stop_cop = store.get<ContainerOperationFactoryAPI>("_ECContainerStruct:activate_state")->create<OperationAPI>(container->fullName() + ":" +  "activate_state_" + "stopped" + ".ope", 
      { {"defaultArg", 
            { {"stateName", "stopped"} }
      }});
    stop_cop->setOwner(container);
    if (stop_cop->isNull()) {
        return Value::error(logger::error("createEC failed. ContainerOperation can not be created"));
    }

    auto start_cop = store.get<ContainerOperationFactoryAPI>("_ECContainerStruct:activate_state")->create<OperationAPI>(container->fullName() + ":" +  "activate_state_" + "started" + ".ope", 
      { {"defaultArg", 
            { {"stateName", "started"} }
      }});
    if (start_cop->isNull()) {
        return Value::error(logger::error("createEC failed. ContainerOperation can not be created"));
    }
    start_cop->setOwner(container);

    store.add<ContainerAPI>(c);
    c->addOperation(start_cop);
    c->addOperation(stop_cop);
    c->addOperation(getter);
    return c->info();
}
