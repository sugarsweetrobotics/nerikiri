#include <sstream>
#include "nerikiri/nerikiri.h"
#include "nerikiri/functional.h"
#include "nerikiri/process.h"
#include "nerikiri/logger.h"
#include "nerikiri/signal.h"
#include "nerikiri/naming.h"
#include "nerikiri/argparse.h"
#include "nerikiri/processconfigparser.h"


#include <iostream>

using namespace nerikiri;
using namespace nerikiri::logger;

Value defaultProcessConfig({
  {"modules", {
    {"operations", {
      {"preload", Value::list()}
    }},
    {"containers", {
      {"preload", Value::list()}
    }},
    {"containerOperations", {
      {"preload", Value::list()}
    }},
    {"ecs", {
      {"preload", Value::list()}
    }},
    {"brokers", {
      {"preload", Value::list()}
    }}
  }}
});

Process Process::null("");

Process::Process(const int argc, const char** argv) : info_({{"name", argv[0]}}), store_(this), config_(defaultProcessConfig) {
  logger::info("Process::Process(\"{}\")", argv[0]);
  std::string fullpath = argv[0];
  std::string path = fullpath.substr(0, fullpath.rfind("/")+1);

  ArgParser parser;
  parser.option<std::string>("-f", "--file", "Setting file path", false, "nk.json");
  auto options = parser.parse(argc, argv);
  
  setExecutablePath(path);
  parseConfigFile(options.get<std::string>("file"));
}

Process::Process(const std::string& name) : info_({{"name", name}}), store_(this) {
  logger::trace("Process::Process(\"{}\")", name);
  if (name.length() == 0) { 
    info_ = Value::error("Process is Null."); 
  } 
}

Process::~Process() {
  logger::trace("Process::~Process()");
}

void Process::_preloadOperations() {
  auto c = config_.at("operations").at("preload");
  c.list_for_each([this](auto& value) {
    this->loadOperationFactory({{"name", value}});
  });

  c = config_.at("operations").at("precreate");
  c.list_for_each([this](auto& value) {
    this->createOperation({{"name", value}});
  });
}


void Process::_preloadContainers() {
  auto c = config_.at("containers").at("preload");
  c.object_for_each([this](auto& key, auto& value) {
    this->loadContainerFactory({{"name", key}});
    value.list_for_each([this, &key](auto& v) {
      //auto n = key + "_" + v.stringValue();
      this->loadContainerOperationFactory({{"name", v}, {"container_name", key}});
    });
  });

  c = config_.at("containers").at("precreate");
  c.object_for_each([this](auto& key, auto& value) {
    auto cinfo = createContainer({{"name", key}});
    auto cont = this->store()->getContainer(cinfo);
    value.list_for_each([this, &cont](auto& v) {
      cont.createContainerOperation({{"name", v}});
    });
  });
}


void Process::parseConfigFile(const std::string& filepath) {

  std::FILE* fp = nullptr;

  /// if path is starts with "/", fullpath.
  if (filepath.at(1) == '/') {
    fp = std::fopen(filepath.c_str(), "r");
  }
  /// else (relative path) 
  else {
    /// Current Directory
    fp = std::fopen(filepath.c_str(), "r");
    if (!fp) {
      /// Directory same as executable
      std::string fullPath = this->path_ + filepath;
      fp = std::fopen(fullPath.c_str(), "r");
    }
  }

  if (fp) {
    config_ = merge(ProcessConfigParser::parseConfig(fp), config_);
  } else {
    logger::info("Process::parseConfigFile. Can not open file ({})", filepath);
  }

  logger::info("Process::parseConfigFile -> {}", config_);
}

Process& Process::addSystemEditor(SystemEditor_ptr&& se) {
  logger::trace("Process::addSystemEditor()");
  systemEditors_.emplace(se->name(), std::forward<SystemEditor_ptr>(se));
  return *this;
}

Value Process::addBroker(const Broker_ptr& brk) {
  logger::trace("Process::addBroker()");
  if (brk) {
    brk->setProcess(Process_ptr(this));
    brk->setProcessStore(&store_);
    auto name = nerikiri::numbering_policy<Broker_ptr>(brokers_, brk->info().at("name").stringValue(), ".brk");
    brk->setInstanceName(name);
    brokers_.push_back(brk);
    return brk->info();
  } 
  return Value::error(logger::error("addBroker failed. Added broker is null."));
}

static auto start_broker(std::vector<std::thread>& threads, Process* process, Broker_ptr& brk) {
  logger::trace("Creating a thread for broker {}", brk->getBrokerInfo());
  std::promise<bool> prms;
  auto ftr = prms.get_future();
  threads.emplace_back(std::thread([&brk, process](std::promise<bool> p) {
				     std::stringstream ss;
				     ss << std::this_thread::get_id();
				     logger::trace("A thread {} is going to run broker {}", ss.str(), str(brk->getBrokerInfo()));
				     p.set_value(brk->run(process));
				     logger::trace("A thread {} finished", ss.str());
				   }, std::move(prms)));
  return ftr;
}

static std::future<bool> start_systemEditor(std::vector<std::thread>& threads, const SystemEditor_ptr& se) {
  logger::trace("Creating a thread for SystemEditor {}", se->name());
  std::promise<bool> prms;
  auto ftr = prms.get_future();
  threads.emplace_back(std::thread([&se](std::promise<bool> p) {
				     std::stringstream ss;
				     ss << std::this_thread::get_id();
				     logger::trace("A thread {} is going to run SystemEditor {}", ss.str(), se->name());
				     p.set_value(se->run());
				     logger::trace("A thread {} finished", ss.str());
				   }, std::move(prms)));
  return ftr;
}

void Process::startAsync() {
  logger::trace("Process::startAsync()");
  auto broker_futures = nerikiri::map<std::future<bool>, Broker_ptr>(brokers_, [this](auto& brk) -> std::future<bool> {
								   return start_broker(this->threads_, this, brk);
								 });
  
  auto systemEditor_futures = nerikiri::map<std::future<bool>, std::string, SystemEditor_ptr>(this->systemEditors_,
											      [this](const auto& name, const auto& se) {
												return start_systemEditor(this->threads_, se); }
											      );

}
  
int32_t Process::wait() {
  logger::trace("Process::wait()");
  if (!wait_for(SIGNAL_INT)) {
    logger::error("Process::start method try to wait for SIGNAL_INT, but can not.");
    return -1;
  }
  return 0;
}

void Process::shutdown() {
  logger::trace("Process::shutdown()");
  nerikiri::foreach<Broker_ptr>(brokers_, [this](auto& brk) {
			      brk->shutdown(this);
			    });
  nerikiri::foreach<std::string, SystemEditor_ptr>(systemEditors_, [this](auto& name, auto& se) {
				      se->shutdown();
				    });
  
  logger::trace(" - joining...");
  for(auto& t : this->threads_) {
    t.join();
  }
  logger::trace(" - joined");
}


int32_t Process::start() {
  logger::trace("Process::start()");
  _preloadOperations();
  _preloadContainers();
  if (on_starting_) on_starting_(this);
  startAsync();
  if (on_started_) on_started_(this);
  if (wait() < 0) return -1;
  shutdown();
  return 0;
}

Broker_ptr Process::getBrokerByName(const std::string& name) {
  for(auto& brk : brokers_) {
    if (brk->getBrokerInfo().at("name").stringValue() == name) {
      return brk;
    }
  }
  return Broker::null;
}

Broker_ptr Process::createBrokerProxy(const BrokerInfo& bi) {
  logger::trace("Process::createBrokerProxy({})", (bi));
  for(auto f : brokerFactories_) {
    if(f->typeName() == bi.at("name").stringValue()) {
      logger::info("Creating BrokerProxy ({})", (bi));
      return f->createProxy(bi);
    }
  }
  logger::error("createBroker failed. Can not found appropreate broker factory.");
  return nullptr;
}

void Process::executeOperation(const OperationInfo& info) const {
  logger::trace("Process::executeOpration({})", (info));
  
}

Value Process::createContainer(const  Value& info) {
  logger::trace("Process::createContainer({})", (info));
  auto f = store()->getContainerFactory(info);
  if (!f) {
    return Value::error(logger::error("createContainer failed. Can not find appropreate container factory."));
  }
  logger::info("Creating Container({})", (info));
  return store()->addContainer(f->create());
}

Value Process::createOperation(const OperationInfo& info) {
  logger::trace("Process::createOperation({})", (info));
  auto f = store()->getOperationFactory(info);
  if (!f) {
    return Value::error(logger::error("createOperation failed. Can not find appropreate operation factory."));
  }
  logger::info("Creating Operation({})", (info));
  return store()->addOperation(f->create());
}
    

Value Process::createBroker(const BrokerInfo& ci) {
  logger::trace("Process::createBroker({})", (ci));
  for(auto f : brokerFactories_) {
    if(f->typeName() == ci.at("name").stringValue()) {
      logger::info("Creating Broker ({})", (ci));
      return addBroker(f->create(ci));
    }
  }
  return Value::error(logger::error("createBroker failed. Can not found appropreate broker factory."));
}
  
Value Process::createExecutionContext(const Value& value) {
  auto f = store()->getExecutionContextFactory(value);
  if (!f) {
    return Value::error(logger::error("createExecutionContext failed. Can not find appropreate execution context factory."));
  }
  logger::info("Creating Execution Context({})", (value));
  return store()->addExecutionContext(f->create(value));
}

/**
 * もしConnectionInfoで指定されたBrokerが引数のbrokerでなければ，親プロセスに対して別のブローカーをリクエストする
 */
Value checkProcessHasProviderOperation(Process* process, const ConnectionInfo& ci) {
    if (ci.isError()) return ci;    
    if (process->store()->getOperation(ci.at("output").at("info")).isNull()) {
      return Value::error(logger::warn("The broker received makeConnection does not have the provider operation."));
    }
    return ci;
}

Value Process::registerProviderConnection(const ConnectionInfo& ci, BrokerAPI* receiverBroker) {
  logger::trace("Process::registerProviderConnection({})", (ci));
  auto ret = checkProcessHasProviderOperation(this, ci);
  if (ret.isError()) return ret;
  auto& provider = store()->getOperation(ret.at("output").at("info"));
  if (provider.hasOutputConnectionRoute(ci)) {
      return Value::error(logger::warn("makeConnection failed. Provider already have the same connection route {}", str(ci.at("output"))));
  }
  while (provider.hasOutputConnectionName(ret)) {
      logger::warn("makeConnection failed. Provider already have the same connection route {}", str(ret.at("output")));
      ret["name"] = ci.at("name").stringValue() + "_2";
  }

  auto consumerBroker = createBrokerProxy(ret.at("input").at("broker"));
  if (!consumerBroker) {
    return Value::error(logger::error("makeConnection failed. Consumer side broker can not be created. {}", str(ci.at("output"))));
  }

  auto ret2 = consumerBroker->registerConsumerConnection(ret);
  if(ret2.isError()) return ret;

  // リクエストが成功なら、こちらもConnectionを登録。
  auto ret3 = provider.addProviderConnection(providerConnection(ret2, createBrokerProxy(ret2.at("input").at("broker"))));
  if (ret3.isError()) {
      // 登録が失敗ならConsumer側のConnectionを破棄。
      auto ret3 = nerikiri::removeConsumerConnection(createBrokerProxy(ret2.at("input").at("broker")), ret2);
      return Value::error(logger::error("request registerProviderConnection for provider's broker failed. ", ret2.getErrorMessage()));
  }// 登録成功ならciを返す
  return ret2;
}

Value Process::registerConsumerConnection(const ConnectionInfo& ci) {
  logger::trace("Process::registerConsumerConnection({}", (ci));
  if (ci.isError()) return ci;

  auto ret = ci;
  /// Consumer側でなければ失敗出力
  auto& consumer = store()->getOperation(ci.at("input").at("info"));
  if (consumer.isNull()) {
      return Value::error(logger::error("registerConsumerConnection failed. The broker does not have the consumer ", str(ci.at("input"))));
  }
  if (consumer.hasInputConnectionRoute(ret)) {
      return Value::error(logger::error("registerConsumerConnection failed. Consumer already have the same connection.", str(ci.at("input"))));
  }
  while (consumer.hasInputConnectionName(ret)) {
      logger::warn("registerConsumerConnection failed. Consumer already have the same connection.", (ret.at("input")));
      ret["name"] = ret["name"].stringValue() + "_2";
  }

  return consumer.addConsumerConnection(consumerConnection(ret,
             createBrokerProxy(ci.at("output").at("broker"))));
}


Value Process::deleteProviderConnection(const ConnectionInfo& ci) {
    logger::trace("Process::deleteProviderConnection({}", (ci));
    if (ci.isError()) return ci;

    auto& provider = store()->getOperation(ci.at("output").at("info"));
    const auto& connection = provider.getOutputConnection(ci);

    auto ret = connection.info();
    auto consumerBroker = createBrokerProxy(ret.at("input").at("broker"));
    if (!consumerBroker) {
      return Value::error(logger::error("Process::deleteProviderConnection failed. ConsumerBrokerProxy cannot be created."));
    }
    ret = consumerBroker->removeConsumerConnection(ret);
    if (ret.isError()) {
      return Value::error(logger::error("Process::deleteProviderConnection failed. RemoveConsumerConnection failed."));
    }
    return provider.removeProviderConnection(ret);
}

Value Process::deleteConsumerConnection(const ConnectionInfo& ci) {
  logger::trace("Process::deleteConsumerConnection({}", (ci));
  if (ci.isError()) return ci;
  auto& op = store()->getOperation(ci.at("input").at("info"));
  return op.removeConsumerConnection(ci);
}


Value Process::putToArgument(const Value& opInfo, const std::string& argName, const Value& value) {
  logger::trace("Process::putToArgument({})", (opInfo));
  auto& op = store()->getOperation(opInfo);
  return op.putToArgument(argName, value);
}

Value Process::putToArgumentViaConnection(const Value& conInfo, const Value& value) {
  logger::trace("Process::putToArgumentViaConnection({})", (conInfo));
  auto& op = store()->getOperation(conInfo.at("input").at("info"));
  return op.putToArgumentViaConnection(conInfo, value);  
}

Value Process::bindECtoOperation(const std::string& ecName, const Value& opInfo) {
  if (opInfo.isError()) {
    logger::error("Process::bindECtoOperation failed. Operation Information is Error. ({})", (opInfo));
    return opInfo;
  }
  auto ec = store()->getExecutionContext({{"instanceName", ecName}});
  if (ec) {
    return ec->bind(std::reference_wrapper<OperationBaseBase>(getOperation(opInfo)));
  } else {
    return Value::error(logger::error("Process::bindECtoOperation failed. EC can not be found ({})", ecName));
  }
}


Value Process::loadOperationFactory(const Value& info) {
  logger::trace("Process::loadOperationFactory({})",(info));
  auto name = info.at("name").stringValue();

  std::vector<std::string> search_paths{"./", path_};
  for(auto& p : search_paths) {
    
    auto dllproxy = createDLLProxy(p, name);
    if (dllproxy) {
      dllproxies_.push_back(dllproxy);
      auto f = dllproxy->functionSymbol(info.at("name").stringValue());
      if (f) {
          addOperationFactory(std::shared_ptr<OperationFactory>(  static_cast<OperationFactory*>(f())  ) );
          return info;
      } 
      //return Value::error(logger::error("Process::loadOperationFactory failed. Can not load Symbol ({})", str(info)));
    }
  }
  return Value::error(logger::error("Process::loadOperationFactory failed. Can not load DLL ({})", str(info)));
}

Value Process::loadContainerOperationFactory(const Value& info) {
  logger::trace("Process::loadContainerOperationFactory({})", (info));
  auto name = info.at("container_name").stringValue() + "_" + info.at("name").stringValue();


  std::vector<std::string> search_paths{"./", path_};

  for(auto& p : search_paths) {
    auto dllproxy = createDLLProxy(p, name);
    if (dllproxy) {
      dllproxies_.push_back(dllproxy);
      auto f = dllproxy->functionSymbol(name);
      if (f) {
          addContainerOperationFactory(std::shared_ptr<ContainerOperationFactoryBase>(  static_cast<ContainerOperationFactoryBase*>(f())  ) );
          return info;
      } 
      //return Value::error(logger::error("Process::loadContainerOperationFactory failed. Can not load Symbol ({})", str(info)));
    }
  }
  return Value::error(logger::error("Process::loadContainerOperationFactory failed. Can not load DLL ({})", str(info)));
}

Value Process::loadContainerFactory(const Value& info) {
  logger::trace("Process::loadContainerFactory({})", (info));
  auto name = info.at("name").stringValue();

  std::vector<std::string> search_paths{"./", path_};

  for(auto& p : search_paths) {
    auto dllproxy = createDLLProxy(p, name);
    if (dllproxy) {
      dllproxies_.push_back(dllproxy);
      auto f = dllproxy->functionSymbol("create" + name);
      if (f) {
          addContainerFactory(std::shared_ptr<ContainerFactoryBase>(  static_cast<ContainerFactoryBase*>(f())  ) );
          return info;
      } 
      //return Value::error(logger::error("Process::loadContainerFactory failed. Can not load Symbol ({})", str(info)));
    }
  }
  return Value::error(logger::error("Process::loadContainerFactory failed. Can not load DLL ({})", str(info)));
}
