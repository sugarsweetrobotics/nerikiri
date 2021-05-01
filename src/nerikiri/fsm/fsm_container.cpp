

#include <algorithm>
#include <nerikiri/functional.h>
#include <nerikiri/container.h>
#include <nerikiri/container_factory.h>
#include <nerikiri/container_operation_factory.h>
#include <nerikiri/logger.h>
#include "nerikiri/fsm/fsm_container.h"

using namespace nerikiri;

struct _FSMContainerStruct {
    std::string fullName;
    std::string currentState;
    std::vector<std::string> availableStates;
    std::map<std::string, std::vector<std::string>> transittableStates;
    _FSMContainerStruct(): currentState("") {}
};


bool 
nerikiri::setupFSMContainer(nerikiri::ProcessStore& store) {
    store.add<ContainerFactoryAPI>(std::shared_ptr<ContainerFactoryAPI>(static_cast<ContainerFactoryAPI*>(containerFactory<_FSMContainerStruct>())));
    store.add<ContainerOperationFactoryAPI>(std::shared_ptr<ContainerOperationFactoryAPI>(static_cast<ContainerOperationFactoryAPI*>(containerOperationFactory<_FSMContainerStruct>(
        {
          {"typeName", "activate_state"},
          {"defaultArg", {
              {"stateName", "__invalid__"}
          }},
        },
        [](auto& container, auto arg) {
            if (!arg.hasKey("stateName")) {
                return Value::error(logger::error("FSMContainer::activate_state failed. Argument does not has 'stateName' object"));
            }
            const auto s = Value::string(arg["stateName"]);
            const auto c = container.currentState;
            if (std::find(container.availableStates.begin(), container.availableStates.end(), s) == container.availableStates.end()) {
                return Value::error(logger::error("[FSMStateContainer] failed to activate {} in FSM ({}). This is not found in available state list.", s, container.fullName));
            }
            if (std::find(container.transittableStates[c].begin(), container.transittableStates[c].end(), s) == container.transittableStates[c].end()) {
                return Value::error(logger::error("[FSMStateContainer] failed to activate {} in FSM ({}). This is not found in transittable state list.", s, container.fullName));
            }
            logger::trace("FSMContainerStruct::activate_state_{} succeeded", s);
            container.currentState = s;
            return arg;
        }))
    ));

    store.add<ContainerOperationFactoryAPI>(std::shared_ptr<ContainerOperationFactoryAPI>(static_cast<ContainerOperationFactoryAPI*>(containerOperationFactory<_FSMContainerStruct>(
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


std::shared_ptr<OperationAPI> createFSMStateContainerOperation(ProcessStore& store, const Value& stateInfo, const std::shared_ptr<Container<_FSMContainerStruct>>& container) {

    /// きちんとstateInfoがnameとtransit要素を持っているかチェックする
    if (!stateInfo.hasKey("name")) {
        logger::error("FSM creating FSMState in FSM::_setupStates(info) failed. Each FSM state info must have 'name' string type member to specify the name of the state");
        return nullOperation();
    }
    if (!stateInfo.hasKey("transit") || !stateInfo.at("transit").isListValue()) {
        logger::error("FSM creating FSMState in FSM::_setupStates(info) failed. Each FSM state info must have 'transit' LIST type member to specify the transitions of the state");
        return nullOperation();
    }
    
    // ここからStateを実際に作ります．
    const auto name = stateInfo.at("name").stringValue();
    auto cop = store.get<ContainerOperationFactoryAPI>("_FSMContainerStruct:activate_state")->create<OperationAPI>(container->fullName() + ":" +  "activate_state_" + name + ".ope", 
      { {"defaultArg", 
            {
                {"stateName", name}
            }
      }});
    if (cop->isNull()) {
        logger::error("createFSM failed. ContainerOperation can not be created");
        return nullOperation();
    }
    cop->setOwner(container);

    container->ptr()->availableStates.push_back(name);
    container->ptr()->transittableStates[name] = stateInfo["transit"].template const_list_map<std::string>([](auto info) {return info.stringValue(); });
    return cop; 
}

Value nerikiri::createFSM(ProcessStore& store, const std::string& fullName, const Value& fsmInfo) {
    logger::info("nerikiri::createFSM({})", fsmInfo);
    //auto fullName = loadFullName(store.fsms(), fsmInfo);
    // まずコンテナ作成
    auto container = store.get<ContainerFactoryAPI>("_FSMContainerStruct")->create(fullName);
    if (container->isNull()) {
        return Value::error(logger::error("ObjectFactory::createFSM failed. NullContainer is returned from factory."));
    }
    auto c = std::dynamic_pointer_cast<Container<_FSMContainerStruct>>(container);
    if (!c) {
        return Value::error(logger::error("ObjectFactory::createFSM failed. Failed to cast to Container<_FSMContainerStruct>."));
    }

    /// デフォルト状態も必須要素
    if (fsmInfo.at("defaultState").isError()) {
        logger::warn("FSM::FSM(info) failed. FSM default state is not specified.");
        logger::trace("FSM::{} exit", __func__);
        return false;
    }
    auto defaultStateName = Value::string(fsmInfo.at("defaultState"));

    c->ptr()->fullName = fullName;
    c->ptr()->currentState = defaultStateName;

    auto getter = store.get<ContainerOperationFactoryAPI>("_FSMContainerStruct:get_state")->create<OperationAPI>(container->fullName() + ":" + "get_state.ope");
    getter->setOwner(container);

    bool failedFlag = false;
    auto ops = fsmInfo["states"].const_list_map<std::shared_ptr<OperationAPI>>([&failedFlag, &store, c, fullName](auto stateInfo) -> std::shared_ptr<OperationAPI> {
        auto cop = createFSMStateContainerOperation(store, stateInfo, c);
        if (cop->isNull()) failedFlag = true;
        return cop;
    });
    if (failedFlag) {
        return Value::error(logger::error("createFSM failed. Creating States make errors"));
    }

    // store.addContainer(c, ".fsm");
    store.add<ContainerAPI>(c);
    std::for_each(ops.begin(), ops.end(), [&c, &store](auto op) {
        //store.addOperation(op);
        c->addOperation(op);
    });
    //store.addOperation(getter);
    c->addOperation(getter);
    return c->info();
}
