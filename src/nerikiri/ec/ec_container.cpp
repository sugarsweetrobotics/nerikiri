
#include <algorithm>
#include <nerikiri/functional.h>
#include <nerikiri/container.h>
#include <nerikiri/container_factory.h>
#include <nerikiri/container_operation_factory.h>
#include <nerikiri/logger.h>
#include "nerikiri/ec/ec_container.h"

using namespace nerikiri;

struct _ECContainerStruct {
    std::string fullName;
    std::string currentState;
    const std::vector<std::string> availableStates;
    _ECContainerStruct(): currentState(""), availableStates({"started", "stopped"}) {}
};


bool 
nerikiri::setupECContainer(nerikiri::ProcessStore& store) {
    store.addContainerFactory(std::shared_ptr<ContainerFactoryAPI>(static_cast<ContainerFactoryAPI*>(containerFactory<_ECContainerStruct>())));
    store.addContainerOperationFactory(std::shared_ptr<ContainerOperationFactoryAPI>(static_cast<ContainerOperationFactoryAPI*>(containerOperationFactory<_ECContainerStruct>(
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

    store.addContainerOperationFactory(std::shared_ptr<ContainerOperationFactoryAPI>(static_cast<ContainerOperationFactoryAPI*>(containerOperationFactory<_ECContainerStruct>(
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
    logger::info("nerikiri::createEC({})", fsmInfo);
    //auto fullName = loadFullName(store.fsms(), fsmInfo);
    auto container = store.containerFactory("_ECContainerStruct")->create(fullName);
    if (container->isNull()) {
        return Value::error(logger::error("ObjectFactory::createEC failed. NullContainer is returned from factory."));
    }
    auto c = std::dynamic_pointer_cast<Container<_ECContainerStruct>>(container);
    if (!c) {
        return Value::error(logger::error("ObjectFactory::createEC failed. Failed to cast to Container<_ECContainerStruct>."));
    }

    auto defaultStateName = "stopped";

    c->ptr()->fullName = fullName;
    c->ptr()->currentState = defaultStateName;

    auto getter = store.containerOperationFactory("_ECContainerStruct", "get_state")->create(container, container->fullName() + ":" + "get_state.ope");

    auto stop_cop = store.containerOperationFactory("_ECContainerStruct", "activate_state")->create(container, container->fullName() + ":" +  "activate_state_" + "stopped" + ".ope", 
      { {"defaultArg", 
            { {"stateName", "stopped"} }
      }});
    if (stop_cop->isNull()) {
        logger::error("createEC failed. ContainerOperation can not be created");
        return nullOperation();
    }

    auto start_cop = store.containerOperationFactory("_ECContainerStruct", "activate_state")->create(container, container->fullName() + ":" +  "activate_state_" + "started" + ".ope", 
      { {"defaultArg", 
            { {"stateName", "started"} }
      }});
    if (start_cop->isNull()) {
        logger::error("createEC failed. ContainerOperation can not be created");
        return nullOperation();
    }

    store.addContainer(c, ".ec");
    store.addOperation(start_cop);
    store.addOperation(stop_cop);
    store.addOperation(getter);
    return c->info();
}