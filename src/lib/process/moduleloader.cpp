#include <juiz/logger.h>
#include <juiz/utils/dllproxy.h>

#include "moduleloader.h"

#include <juiz/container_api.h>
#include <juiz/operation.h>


using namespace juiz;

Value ModuleLoader::loadOperationFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("ModuleLoader::loadOperationFactory({})",(info));
  auto name = info.at("typeName").stringValue();
    if (info.hasKey("load_paths")) {
    info.at("load_paths").const_list_for_each([&search_paths](auto& value) {
      search_paths.push_back(value.stringValue());
    });
  }
  for(auto& p : search_paths) {   
    logger::trace(" - loading {}, {}", p, name);
    auto dllproxy = createDLLProxy(p, name);
    if (!dllproxy->isNull()) {
      store.addDLLProxy(dllproxy);
      auto f = dllproxy->functionSymbol(info.at("typeName").stringValue());
      if (f) {
        logger::debug("ModuleLoader::loadOperationFactory({}, {}) load success.", p, name);
        //store.addOperationFactory(std::shared_ptr<OperationFactoryAPI>(  static_cast<OperationFactoryAPI*>(f())  ) );
        store.add<OperationFactoryAPI>(std::shared_ptr<OperationFactoryAPI>(  static_cast<OperationFactoryAPI*>(f())  ) );
        return info;
      } else {
        logger::debug("ModuleLoader:loadOperationFactory failed. Can load DLL but can not find Symbol({})", name);
      }
    } else {
      logger::debug("ModuleLoader::loadOperationFactory failed. Can not load DLL file {}, {}", p, name);
    }
  }
  return Value::error(logger::error("ModuleLoader::loadOperationFactory failed. Can not load DLL ({})", str(info)));
}


std::shared_ptr<ContainerOperationFactoryAPI> ModuleLoader::loadContainerOperationFactory(const std::shared_ptr<DLLProxy>& dllproxy, const std::string& typeName) {
  auto f = dllproxy->functionSymbol(typeName);
  if (f) {
    logger::debug("ModuleLoader::loadContainerOperationFactory({}) load success.", typeName);
    return std::shared_ptr<ContainerOperationFactoryAPI>(  static_cast<ContainerOperationFactoryAPI*>(f()) );
  } 
  
  logger::error("ModuleLoader::loadContainerOperationFactory failed. Can load DLL but can not find Symbol({})", typeName);
  return nullObject<ContainerOperationFactoryAPI>();
}

Value ModuleLoader::loadContainerOperationFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  Value v = Value::list();
  for ( auto s : search_paths) { v.push_back(s); }
  logger::trace("ModuleLoader::loadContainerOperationFactory(search_paths={}, info={})", v, info);
  auto name = info.at("container_name").stringValue() + "_" + info.at("typeName").stringValue();
  if (info.hasKey("load_paths")) {
    info.at("load_paths").const_list_for_each([&search_paths](auto& value) {
      search_paths.push_back(value.stringValue());
    });
  }
  Value search_paths_info = Value::list();
  for(auto& p : search_paths) {
    search_paths_info.push_back(p);
    auto dllproxy = createDLLProxy(p, name);
    if (!dllproxy->isNull()) {
      store.addDLLProxy(dllproxy);
      auto f = dllproxy->functionSymbol(name);
      if (f) {
        logger::info("ModuleLoader::loadContainerOperationFactory({}, {}) load success.", p, name);
        store.add<ContainerOperationFactoryAPI>(std::shared_ptr<ContainerOperationFactoryAPI>(  static_cast<ContainerOperationFactoryAPI*>(f())  ) );
        return info;
      } else {
        logger::debug("ModuleLoader::loadContainerOperationFactory failed. Can load DLL but can not find Symbol({})", name);
      }
    } else {
      logger::debug("ModuleLoader::loadContainerOperationFactory failed. Can not load DLL file {}, {}", p, name);
    }
  }
  return Value::error(logger::error("ModuleLoader::loadContainerOperationFactory(search_paths={}, info={}) failed. Can not load DLL (fileName={})", search_paths_info, info, name));
}

std::shared_ptr<DLLProxy> ModuleLoader::loadDLL(const std::string& typeName, const std::vector<std::string>& search_paths) {
  logger::trace("ModuleLoader::loadDLL({}, search_paths=[...]) called", typeName);
  for(auto& p : search_paths) {
    auto dllproxy = createDLLProxy(p, typeName);
    if (dllproxy->isNull()) {
      logger::debug("ModuleLoader::loadDLL failed. Can not load DLL from path (p={})", p);
      continue;
    }
    logger::debug("ModuleLoader::loadDLL success (path={})", p);
    return dllproxy;
  }
  return nullptr;
}



std::shared_ptr<ContainerFactoryAPI> ModuleLoader::loadContainerFactory(const std::shared_ptr<DLLProxy>& dllproxy, const std::string& typeName) {
  logger::trace("ModuleLoader::loadContainerFactory({}, search_paths) called", typeName);
  auto f = dllproxy->functionSymbol("create" + typeName); 
  if (f) {
    logger::debug("ModuleLoader::loadContainerFactory({}). function(\"create{}\") is successfully loaded.", typeName,typeName);
    return std::shared_ptr<ContainerFactoryAPI>(static_cast<ContainerFactoryAPI*>(f()));
  } 
  logger::error("ModuleLoader::loadContainerFactory failed. Can not load function(\"create{}\")", typeName);
  return nullObject<ContainerFactoryAPI>();
}
 

Value ModuleLoader::loadContainerFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("ModuleLoader::loadContainerFactory({})", (info));
  auto name = info.at("typeName").stringValue();
  if (info.hasKey("load_paths")) {
    info.at("load_paths").const_list_for_each([&search_paths](auto& value) {
      search_paths.push_back(value.stringValue());
    });
  }
  for(auto& p : search_paths) {
    auto dllproxy = createDLLProxy(p, name);
    if (!dllproxy->isNull()) {
      store.addDLLProxy(dllproxy);
      auto f = dllproxy->functionSymbol("create" + name); 
      if (f) {
        logger::debug("ModuleLoader::loadContainerFactory({}, {}). function(\"create{}\") is successfully loaded.", p, name, name);
        auto cf = static_cast<ContainerFactoryAPI*>(f());
        store.add<ContainerFactoryAPI>(std::shared_ptr<ContainerFactoryAPI>( cf ) );
        return info;
      } else {
        logger::debug("ModuleLoader::loadContainerFactory failed. Can load DLL but can not find Symbol({})", name);
      }
    } else {
      logger::debug("ModuleLoader::loadContainerFactory failed. Can not load DLL ({})", info);
    }
  }
  return Value::error(logger::error("ModuleLoader::loadContainerFactory failed. Can not load DLL ({})", str(info)));
}


Value ModuleLoader::loadExecutionContextFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("ModuleLoader::loadExecutionContextFactory({})", (info));
  auto name = info.at("typeName").stringValue();
  if (info.hasKey("load_paths")) {
    info.at("load_paths").const_list_for_each([&search_paths](auto& value) {
      search_paths.push_back(value.stringValue());
    });
  }
  for(auto& p : search_paths) {
    auto dllproxy = createDLLProxy(p, name);
    if (!dllproxy->isNull()) {
      store.addDLLProxy(dllproxy);
      auto f = dllproxy->functionSymbol("create" + name);
      if (f) {
        logger::debug("ModuleLoader::loadExecutionContextFactory({}, {}) load success.", p, name);
        store.addECFactory(std::shared_ptr<ExecutionContextFactoryAPI>(  static_cast<ExecutionContextFactoryAPI*>(f())  ) );
        return info;
      } else {
          logger::debug("ModuleLoader::loadExecutionContextFactory failed. Can load DLL but can not find Symbol({})", "create" + name);
      }
    } else {
      logger::debug("ModuleLoader::loadExecutionContextFactory failed. Can not load DLL ({})", info);
    }
  }
  return Value::error(logger::error("ModuleLoader::loadExecutionContextFactory failed. Can not load DLL ({})", str(info)));
}

Value ModuleLoader::loadBrokerFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info_) {
  logger::trace("ModuleLoader::loadBrokerFactory({}) entry", (info_));
  auto name = Value::string(info_.at("typeName"));
  info_.at("load_paths").const_list_for_each([&search_paths](auto& value) {
    search_paths.push_back(value.stringValue());
  });
  for(auto p : search_paths) {
    logger::trace(" - for search path: {}", p);

    auto dllproxy = createDLLProxy(p, name);
    if (dllproxy->isNull()) {
      logger::debug("ModuleLoader::loadBrokerFactory failed. Can not load DLL ({})", info_);
      continue;
    }
    
    auto f = dllproxy->functionSymbol("create" + name);
    if (f) {
      logger::debug("ModuleLoader::loadBrokerFactory({}, {}) load success. Function symbol({}) can be acquired.", p, name, "create" + name);
      store.addDLLProxy(dllproxy);
      store.addBrokerFactory(std::shared_ptr<BrokerFactoryAPI>(  static_cast<BrokerFactoryAPI*>(f())  ) );
      logger::trace("ModuleLoader::loadBrokerFactory({}) exit", (info_));
      return info_;
    } else {
      logger::debug("ModuleLoader::loadBrokerFactory failed. Can load DLL but can not find Symbol({})", "create" + name);
    }
    
  }

  auto info = Value::error(logger::error("ModuleLoader::loadBrokerFactory failed. Can not load DLL ({})", str(info_)));
  logger::trace("ModuleLoader::loadBrokerFactory({}) exit", (info));
  return info;
}
