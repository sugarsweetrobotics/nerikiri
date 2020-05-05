#include "nerikiri/util/logger.h"
#include "nerikiri/util/dllproxy.h"

#include "nerikiri/moduleloader.h"

#include "nerikiri/operationfactory.h"


using namespace nerikiri;

Value ModuleLoader::loadOperationFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("Process::loadOperationFactory({})",(info));
  auto name = info.at("name").stringValue();
    if (info.hasKey("load_paths")) {
    info.at("load_paths").list_for_each([&search_paths](auto& value) {
      search_paths.push_back(value.stringValue());
    });
  }
  for(auto& p : search_paths) {   
    auto dllproxy = createDLLProxy(p, name);
    if (dllproxy) {
      store.addDLLProxy(dllproxy);
      auto f = dllproxy->functionSymbol(info.at("name").stringValue());
      if (f) {
          store.addOperationFactory(std::shared_ptr<OperationFactory>(  static_cast<OperationFactory*>(f())  ) );
          return info;
      } 
    }
  }
  return Value::error(logger::error("Process::loadOperationFactory failed. Can not load DLL ({})", str(info)));
}

Value ModuleLoader::loadContainerOperationFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("Process::loadContainerOperationFactory({})", (info));
  auto name = info.at("container_name").stringValue() + "_" + info.at("name").stringValue();
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
          store.addContainerOperationFactory(std::shared_ptr<ContainerOperationFactoryBase>(  static_cast<ContainerOperationFactoryBase*>(f())  ) );
          return info;
      } 
    }
  }
  return Value::error(logger::error("Process::loadContainerOperationFactory failed. Can not load DLL ({})", str(info)));
}

Value ModuleLoader::loadContainerFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("Process::loadContainerFactory({})", (info));
  auto name = info.at("name").stringValue();
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
          store.addContainerFactory(std::shared_ptr<ContainerFactoryBase>(  static_cast<ContainerFactoryBase*>(f())  ) );
          return info;
      } 
    }
  }
  return Value::error(logger::error("Process::loadContainerFactory failed. Can not load DLL ({})", str(info)));
}


Value ModuleLoader::loadExecutionContextFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("Process::loadExecutionContextFactory({})", (info));
  auto name = info.at("name").stringValue();
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
          store.addExecutionContextFactory(std::shared_ptr<ExecutionContextFactory>(  static_cast<ExecutionContextFactory*>(f())  ) );
          return info;
      } 
    }
  }
  return Value::error(logger::error("Process::loadExecutionContextFactory failed. Can not load DLL ({})", str(info)));
}

Value ModuleLoader::loadBrokerFactory(ProcessStore& store, std::vector<std::string> search_paths, const Value& info) {
  logger::trace("Process::loadBrokerFactory({})", (info));
  auto name = info.at("name").stringValue(); 
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
          store.addBrokerFactory(std::shared_ptr<BrokerFactory>(  static_cast<BrokerFactory*>(f())  ) );
          return info;
      } 
    }
  }
  return Value::error(logger::error("Process::loadBrokerFactory failed. Can not load DLL ({})", str(info)));
}
