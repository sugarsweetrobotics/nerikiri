#include <iostream>
#include <algorithm>
#include <juiz/utils/functional.h>
#include <juiz/process_store.h>
#include <juiz/container.h>
#include <juiz/logger.h>
#include "../ec/ec_container.h"
#include "../anchor/anchor_container.h"



using namespace juiz;

class StaticPeriodicAnchor;
std::shared_ptr<ExecutionContextFactoryAPI> staticPeriodicAnchor();

class DynamicPeriodicAnchor;
std::shared_ptr<ExecutionContextFactoryAPI> dynamicPeriodicAnchor();

struct _AnchorContainerStruct {
    std::string fullName;
    std::string typeName;
    std::shared_ptr<ExecutionContextAPI> ec;
    std::string currentState;
    const std::vector<std::string> availableStates;
    _AnchorContainerStruct(): currentState(""), availableStates({"started", "stopped"}) {}
};


bool setupStaticPeriodicAnchorContainer(juiz::ProcessStore& store);
bool setupDynamicPeriodicAnchorContainer(juiz::ProcessStore& store);

bool juiz::setupAnchorContainer(juiz::ProcessStore& store) {
    setupDynamicPeriodicAnchorContainer(store);
    return setupStaticPeriodicAnchorContainer(store);
}

bool setupDynamicPeriodicAnchorContainer(juiz::ProcessStore& store) {
    logger::trace("anchor_container.cpp: setupDynamicPeriodicAnchorContainer() called");
    store.add<ContainerFactoryAPI>(std::shared_ptr<ContainerFactoryAPI>(static_cast<ContainerFactoryAPI*>(containerFactory<_AnchorContainerStruct>({
	    {"className", "DynamicAnchorContainer"}
	  }))));
    

    // activate_state container operation 
    store.add<ContainerOperationFactoryAPI>(std::shared_ptr<ContainerOperationFactoryAPI>(static_cast<ContainerOperationFactoryAPI*>(containerOperationFactory<_AnchorContainerStruct>(
        {
          {"typeName", "activate_state"},
          {"defaultArg", {
            {"stateName", "__invalid__"},
            {"input", {
                {"position", {
                    {"x", 0.0},
                    {"y", 0.0},
                    {"z", 0.0}
                }},
                {"orientation", {
                    {"x", 0.0},
                    {"y", 0.0},
                    {"z", 0.0},
                    {"w", 1.0}
                }}
            }}
          }},
        },
        [](auto& container, auto arg) {
            const auto s = Value::string(arg["stateName"]);
            const auto c = container.currentState;
            if (std::find(container.availableStates.begin(), container.availableStates.end(), s) == container.availableStates.end()) {
                return Value::error(logger::error("[AnchorStateContainer] failed to activate {} in FSM ({}). This is not found in available state list.", s, container.fullName));
            }
            logger::trace("AnchorContainerStruct::activate_state_{} succeeded", s);
            if (c != s) {
                if (s == "started") {
                    container.ec->onStarting();
                } else if (s == "stopped") {
                    container.ec->onStopping();
                }
                container.currentState = s;
                if (s == "started") {
                    container.ec->onStarted();
                } else if (s == "stopped") {
                    container.ec->onStopped();
                }
            }
            
            return Value{
                {"tm", {
                    {"sec", 0},
                    {"nsec", 0}
                }},
                {"pose", arg["input"]}
            };
        }))
    ));

    // get_state container operation
    store.add<ContainerOperationFactoryAPI>(std::shared_ptr<ContainerOperationFactoryAPI>(static_cast<ContainerOperationFactoryAPI*>(containerOperationFactory<_AnchorContainerStruct>(
        {
          {"typeName", "get_state"},
          {"defaultArg", {
          }},
        },
        [](auto& container, auto arg) {
            return container.currentState;
        }))
    ));

    store.addECFactory ( dynamicPeriodicAnchor() );
    return true;
}


bool setupStaticPeriodicAnchorContainer(juiz::ProcessStore& store) {
    logger::trace("anchor_container.cpp: setupStaticPeriodicAnchorContainer() called");
    store.add<ContainerFactoryAPI>(std::shared_ptr<ContainerFactoryAPI>(static_cast<ContainerFactoryAPI*>(containerFactory<_AnchorContainerStruct>({
	    {"className", "StaticAnchorContainer"}
	  }))));
    

    // activate_state container operation 
    store.add<ContainerOperationFactoryAPI>(std::shared_ptr<ContainerOperationFactoryAPI>(static_cast<ContainerOperationFactoryAPI*>(containerOperationFactory<_AnchorContainerStruct>(
        {
          {"typeName", "activate_state"},
          {"defaultArg", {
            {"stateName", "__invalid__"},
            {"offset", {
                {"position", {
                    {"x", 0.0},
                    {"y", 0.0},
                    {"z", 0.0}
                }},
                {"orientation", {
                    {"x", 0.0},
                    {"y", 0.0},
                    {"z", 0.0},
                    {"w", 1.0}
                }}
            }}
          }},
        },
        [](auto& container, auto arg) {
            const auto s = Value::string(arg["stateName"]);
            const auto c = container.currentState;
            if (std::find(container.availableStates.begin(), container.availableStates.end(), s) == container.availableStates.end()) {
                return Value::error(logger::error("[AnchorStateContainer] failed to activate {} in FSM ({}). This is not found in available state list.", s, container.fullName));
            }
            logger::trace("AnchorContainerStruct::activate_state_{} succeeded", s);
            if (c != s) {
                if (s == "started") {
                    container.ec->onStarting();
                } else if (s == "stopped") {
                    container.ec->onStopping();
                }
                container.currentState = s;
                if (s == "started") {
                    container.ec->onStarted();
                } else if (s == "stopped") {
                    container.ec->onStopped();
                }
            }
            
            return Value{
                {"tm", {
                    {"sec", 0},
                    {"nsec", 0}
                }},
                {"pose", arg["offset"]}
            };
        }))
    ));

    // get_state container operation
    store.add<ContainerOperationFactoryAPI>(std::shared_ptr<ContainerOperationFactoryAPI>(static_cast<ContainerOperationFactoryAPI*>(containerOperationFactory<_AnchorContainerStruct>(
        {
          {"typeName", "get_state"},
          {"defaultArg", {
          }},
        },
        [](auto& container, auto arg) {
            return container.currentState;
        }))
    ));

    store.addECFactory ( staticPeriodicAnchor() );
    return true;
}


Value juiz::createDynamicPeriodicAnchor(ProcessStore& store, const std::string& fullName, const Value& info) {
    logger::trace("juiz::createDynamicPeriodicAnchor({}) called", info);

    auto container = store.get<ContainerFactoryAPI>("_AnchorContainerStruct")->create(fullName);
    if (container->isNull()) {
        return Value::error(logger::error("ObjectFactory::createDynamicPeriodicAnchor failed. NullContainer is returned from factory."));
    }
    auto c = std::dynamic_pointer_cast<Container<_AnchorContainerStruct>>(container);
    if (!c) {
        return Value::error(logger::error("ObjectFactory::createDynamicPeriodicAnchor failed. Failed to cast to Container<_AnchorContainerStruct>."));
    }

    auto defaultStateName = "stopped";
    c->ptr()->fullName = fullName;
    c->setClassName("DynamicPeriodicAnchor");
    c->setTypeName(info["typeName"].stringValue());
    c->ptr()->currentState = defaultStateName;

    /// ここでECの本体を設定する
    c->ptr()->ec = store.executionContextFactory(Value::string(info.at("typeName")))->create(info);
    //if (c->ptr()->ec->isNull()) {
    //    return Value::error(logger::error("ObjectFactory::createEC failed. Failed to create ExecutionContext(typeName={})", Value::string(ecInfo.at("typeName"))));
    //}
    auto getter = store.get<ContainerOperationFactoryAPI>("_AnchorContainerStruct:get_state")->create<OperationAPI>(container->fullName() + ":" + "get_state.ope");
    getter->setOwner(container);

    auto stop_cop = store.get<ContainerOperationFactoryAPI>("_AnchorContainerStruct:activate_state")->create<OperationAPI>(container->fullName() + ":" +  "activate_state_" + "stopped" + ".ope", 
      { {"defaultArg", 
            { {"stateName", "stopped"} }
      }});
    stop_cop->setOwner(container);
    if (stop_cop->isNull()) {
        return Value::error(logger::error("createStaticPeriodicAnchor failed. ContainerOperation can not be created"));
    }

    auto start_cop = store.get<ContainerOperationFactoryAPI>("_AnchorContainerStruct:activate_state")->create<OperationAPI>(container->fullName() + ":" +  "activate_state_" + "started" + ".ope", 
        { 
            {"defaultArg", {
                {"stateName", "started"},
                {}
            }}
        });
    if (start_cop->isNull()) {
        return Value::error(logger::error("createStaticPeriodicAnchor failed. ContainerOperation can not be created"));
    }
    start_cop->setOwner(container);

    c->ptr()->ec->setSvcOperation(start_cop);
    

    store.add<ContainerAPI>(c);
    c->addOperation(start_cop);
    c->addOperation(stop_cop);
    c->addOperation(getter);
    c->useThread(false);



    logger::info("Created DynamicPeriodicAnchor(info={})", info);

    return c->info();

}



Value juiz::createStaticPeriodicAnchor(ProcessStore& store, const std::string& fullName, const Value& info) {
    logger::trace("juiz::createStaticPeriodicAnchor({}) called", info);

    auto container = store.get<ContainerFactoryAPI>("_AnchorContainerStruct")->create(fullName);
    if (container->isNull()) {
        return Value::error(logger::error("ObjectFactory::createAnchor failed. NullContainer is returned from factory."));
    }
    auto c = std::dynamic_pointer_cast<Container<_AnchorContainerStruct>>(container);
    if (!c) {
        return Value::error(logger::error("ObjectFactory::createAnchor failed. Failed to cast to Container<_AnchorContainerStruct>."));
    }

    auto defaultStateName = "stopped";
    c->ptr()->fullName = fullName;
    c->setClassName("StaticPeriodicAnchor");
    c->setTypeName(info["typeName"].stringValue());
    c->ptr()->currentState = defaultStateName;

    /// ここでECの本体を設定する
    c->ptr()->ec = store.executionContextFactory(Value::string(info.at("typeName")))->create(info);
    //if (c->ptr()->ec->isNull()) {
    //    return Value::error(logger::error("ObjectFactory::createEC failed. Failed to create ExecutionContext(typeName={})", Value::string(ecInfo.at("typeName"))));
    //}
    auto getter = store.get<ContainerOperationFactoryAPI>("_AnchorContainerStruct:get_state")->create<OperationAPI>(container->fullName() + ":" + "get_state.ope");
    getter->setOwner(container);

    auto stop_cop = store.get<ContainerOperationFactoryAPI>("_AnchorContainerStruct:activate_state")->create<OperationAPI>(container->fullName() + ":" +  "activate_state_" + "stopped" + ".ope", 
      { {"defaultArg", 
            { {"stateName", "stopped"} }
      }});
    stop_cop->setOwner(container);
    if (stop_cop->isNull()) {
        return Value::error(logger::error("createStaticPeriodicAnchor failed. ContainerOperation can not be created"));
    }

    auto start_cop = store.get<ContainerOperationFactoryAPI>("_AnchorContainerStruct:activate_state")->create<OperationAPI>(container->fullName() + ":" +  "activate_state_" + "started" + ".ope", 
        { 
            {"defaultArg", {
                {"stateName", "started"},
                {"offset", info["offset"]}
            }}
        });
    if (start_cop->isNull()) {
        return Value::error(logger::error("createStaticPeriodicAnchor failed. ContainerOperation can not be created"));
    }
    start_cop->setOwner(container);

    c->ptr()->ec->setSvcOperation(start_cop);
    

    store.add<ContainerAPI>(c);
    c->addOperation(start_cop);
    c->addOperation(stop_cop);
    c->addOperation(getter);
    c->useThread(false);


    logger::info("Created StaticPeriodicAnchor(info={})", info);

    return c->info();

}
