#include "nerikiri/logger.h"
#include "nerikiri/dllproxy.h"

#include "nerikiri/moduleloader.h"

#include "nerikiri/operationfactory.h"


using namespace nerikiri;

Value ModuleLoader::loadOperationFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("ModuleLoader::loadOperationFactory({})",(info));
  auto name = info.at("typeName").stringValue();
    if (info.hasKey("load_paths")) {
    info.at("load_paths").list_for_each([&search_paths](auto& value) {
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
        logger::info("ModuleLoader::loadOperationFactory({}, {}) load success.", p, name);
        store.addOperationFactory(std::shared_ptr<OperationFactory>(  static_cast<OperationFactory*>(f())  ) );
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

Value ModuleLoader::loadContainerOperationFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("ModuleLoader::loadContainerOperationFactory({})", (info));
  auto name = info.at("container_name").stringValue() + "_" + info.at("typeName").stringValue();
  if (info.hasKey("load_paths")) {
    info.at("load_paths").list_for_each([&search_paths](auto& value) {
      search_paths.push_back(value.stringValue());
    });
  }
  for(auto& p : search_paths) {
    auto dllproxy = createDLLProxy(p, name);
    if (!dllproxy->isNull()) {
      store.addDLLProxy(dllproxy);
      auto f = dllproxy->functionSymbol(name);
      if (f) {
        logger::info("ModuleLoader::loadContainerOperationFactory({}, {}) load success.", p, name);
        store.addContainerOperationFactory(std::shared_ptr<ContainerOperationFactoryBase>(  static_cast<ContainerOperationFactoryBase*>(f())  ) );
        return info;
      } else {
        logger::debug("ModuleLoader::loadContainerOperationFactory failed. Can load DLL but can not find Symbol({})", name);
      }
    } else {
      logger::debug("ModuleLoader::loadContainerOperationFactory failed. Can not load DLL file {}, {}", p, name);
    }
  }
  return Value::error(logger::error("Process::loadContainerOperationFactory failed. Can not load DLL ({})", str(info)));
}

Value ModuleLoader::loadContainerFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("ModuleLoader::loadContainerFactory({})", (info));
  auto name = info.at("typeName").stringValue();
    if (info.hasKey("load_paths")) {
    info.at("load_paths").list_for_each([&search_paths](auto& value) {
      search_paths.push_back(value.stringValue());
    });
  }
  for(auto& p : search_paths) {
    auto dllproxy = createDLLProxy(p, name);
    if (!dllproxy->isNull()) {
      store.addDLLProxy(dllproxy);
      auto f = dllproxy->functionSymbol("create" + name);
      if (f) {
        logger::info("ModuleLoader::loadContainerFactory({}, {}) load success.", p, name);
        store.addContainerFactory(std::shared_ptr<ContainerFactoryBase>(  static_cast<ContainerFactoryBase*>(f())  ) );
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
    info.at("load_paths").list_for_each([&search_paths](auto& value) {
      search_paths.push_back(value.stringValue());
    });
  }
  for(auto& p : search_paths) {
    auto dllproxy = createDLLProxy(p, name);
    if (!dllproxy->isNull()) {
      store.addDLLProxy(dllproxy);
      auto f = dllproxy->functionSymbol("create" + name);
      if (f) {
        logger::info("ModuleLoader::loadExecutionContextFactory({}, {}) load success.", p, name);
        store.addExecutionContextFactory(std::shared_ptr<ExecutionContextFactory>(  static_cast<ExecutionContextFactory*>(f())  ) );
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

Value ModuleLoader::loadBrokerFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("ModuleLoader::loadBrokerFactory({})", (info));
  auto name = info.at("typeName").stringValue(); 
  if (info.hasKey("load_paths")) {
    info.at("load_paths").list_for_each([&search_paths](auto& value) {
      search_paths.push_back(value.stringValue());
    });
  }
  for(auto& p : search_paths) {
    auto dllproxy = createDLLProxy(p, name);
    if (!dllproxy->isNull()) {
      store.addDLLProxy(dllproxy);
      auto f = dllproxy->functionSymbol("create" + name);
      if (f) {
        logger::info("ModuleLoader::loadBrokerFactory({}, {}) load success.", p, name);
        store.addBrokerFactory(std::shared_ptr<BrokerFactory>(  static_cast<BrokerFactory*>(f())  ) );
        return info;
      } else {
        logger::debug("ModuleLoader::loadBrokerFactory failed. Can load DLL but can not find Symbol({})", "create" + name);
      }
    } else {
      logger::debug("ModuleLoader::loadBrokerFactory failed. Can not load DLL ({})", info);
    }
  }
  return Value::error(logger::error("ModuleLoader::loadBrokerFactory failed. Can not load DLL ({})", str(info)));
}
