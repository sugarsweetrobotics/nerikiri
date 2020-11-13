 #include <sstream>
#include <iostream>


#include "nerikiri/process.h"


#include "nerikiri/argparse.h"
#include "nerikiri/os.h"
#include "nerikiri/signal.h"

#include "nerikiri/processconfigparser.h"
#include "nerikiri/objectfactory.h"
#include "nerikiri/connectionbuilder.h"
#include "nerikiri/moduleloader.h"

using namespace nerikiri;
using namespace nerikiri::logger;


std::map<std::string, std::string> env_dictionary_default{};

Value defaultProcessConfig({
  {"logger", {
    {"logLevel", "OFF"}
  }},
  {"operations", {
    {"precreate", Value::list()},
    {"preload", Value::list()}
  }},
  {"containers", {
    {"precreate", Value::list()},
    {"preload", Value::list()}
  }},
  {"containerOperations", {
    {"precreate", Value::list()},
    {"preload", Value::list()}
  }},
  {"fsms", {
    {"precreate", Value::list()},
    {"preload", Value::list()},
    {"bind", { 
      {"operations", Value::list()},
      {"ecs", Value::list()}
    }}
  }},
  {"ecs", {
    {"precreate", Value::list()},
    {"preload", Value::list()},
    {"bind", Value::list()}
  }},
  {"brokers", {
    {"precreate", Value::list()},
    {"preload", Value::list()}
  }},
  {"connections", {}},
  {"callbacks", Value::list()}
});

Process Process::null("");

Process::Process(const std::string& name) : ProcessAPI("Process", "Process", name), store_(this), config_(defaultProcessConfig), started_(false), env_dictionary_(env_dictionary_default) {
  std::string fullpath = name;
  if (fullpath.find("/") != 0) {
    fullpath = nerikiri::getCwd() + "/" + fullpath;
  }

  std::string path = fullpath.substr(0, fullpath.rfind("/")+1);
  coreBroker_ = std::make_shared<CoreBroker>(this, "coreBroker0");
  try {
    store_.addBrokerFactory(std::make_shared<CoreBrokerFactory>(coreBroker_));
    store_.addTopicFactory(std::make_shared<TopicFactory>("topicFactory"));
    store_.addFSMFactory(std::make_shared<FSMFactory>("fsmFactory"));
    setExecutablePath(getExecutablePath(name));

    env_dictionary_["${ExecutableDirectory}"] = path_.substr(0, path_.rfind('/'));

  } catch (std::exception & ex) {
    logger::error("Process::Process failed. Exception: {}", ex.what());
  }
}

Process::Process(const int argc, const char** argv) : Process(argv[0]) {
  ArgParser parser;
  parser.option<std::string>("-ll", "--loglevel", "Log Level", false, "INFO");
  auto options = parser.parse(argc, argv);
  auto loglevel_str = options.get<std::string>("loglevel");
  logger::setLogLevel(loglevel_str);

  if (options.unknown_args.size() > 0) {
    parseConfigFile(options.unknown_args[0]);
  }
  _setupLogger();
  if (loglevel_str != "INFO") {
    logger::setLogLevel(loglevel_str);
  }

  logger::info("Process::Process(argv[0]=\"{}\")", argv[0]);
  logger::info("Process::Process() - ExecutablePath = {}", this->path_);
  logger::info("ExecutableDirectory = {}", env_dictionary_["${ExecutableDirectory}"]);
}

Process::Process(const std::string& name, const Value& config) : Process(name) {
  config_ = nerikiri::merge(config_, config);  
  _setupLogger();
  logger::info("Process::Process(\"{}\")", name);
  logger::info("Process::Process() - ExecutablePath = {}", path_);
  logger::info("ExecutableDirectory = {}", env_dictionary_["${ExecutableDirectory}"]);
}

Process::Process(const std::string& name, const std::string& jsonStr): Process(name) {
  config_ = merge(config_, ProcessConfigParser::parseConfig(jsonStr));
  _setupLogger();
  logger::info("Process::Process(\"{}\")", name);
}

Process::~Process() {
  logger::trace("Process::~Process()");
  if (started_) {
    stop();
  }
}

void Process::_setupLogger() {
  if (config_.hasKey("logger")) {
    auto loggerConf = config_.at("logger");
    if (loggerConf.hasKey("logLevel")) {
      auto loglevel_str = loggerConf.at("logLevel").stringValue();
      logger::setLogLevel(loglevel_str);
    }
    if (loggerConf.hasKey("logFile")) {
      auto logfile_str = loggerConf.at("logFile").stringValue();
      logger::setLogFileName(logfile_str);
    }
  }

  logger::initLogger();
}

void Process::parseConfigFile(const std::string& filepath) {
  logger::trace("Process::parseConfigFile({}) entry", filepath);

  /// まず読み込んでみてロガーの設定を確認します
  auto fp = fopen(filepath.c_str(), "r");
  if (fp == nullptr) {
    logger::warn("ProcessConfigParser::parseConfig failed. File pointer is NULL.");
  }
  auto v = nerikiri::json::toValue(fp);
  if (v.hasKey("logger")) {
    auto loggerConf = v.at("logger");
    if (loggerConf.hasKey("logLevel")) {
      auto loglevel_str = loggerConf.at("logLevel").stringValue();
      logger::setLogLevel(loglevel_str);
    }
  }

  
  /// ここでプロジェクトを読み込む
  config_ = merge(config_, ProcessConfigParser::parseProjectFile(filepath));
  /// ここで環境変数の辞書の設定
  if (filepath.find("/") != 0) {
    env_dictionary_["${ProjectDirectory}"] = nerikiri::getCwd() + "/";
  } else {
    env_dictionary_["${ProjectDirectory}"] = filepath.substr(0, filepath.rfind("/")+1);
  }
  /// ここで環境変数の辞書の適用
  config_ = replaceAndCopy(config_, env_dictionary_);
  logger::info("Process::parseConfigFile -> {}", config_);

  logger::trace("Process::parseConfigFile({}) exit", filepath);
}

/**
 * OperationFactoryの読み込み
 */
void Process::_preloadOperations() {
  logger::trace("Process::_preloadOperations() entry");
  config_.at("operations").at("preload").const_list_for_each([this](auto value) {
    ModuleLoader::loadOperationFactory(store_, {"./", path_}, {
      {"typeName", value}, {"load_paths", config_.at("operations").at("load_paths")}
    });
  });

  config_.at("operations").at("precreate").const_list_for_each([this](auto& oinfo) {
    ObjectFactory::createOperation(store_, oinfo);
  });


  logger::trace("Process::_preloadOperations() exit");
}


void Process::_preloadContainers() {
  logger::trace("Process::_preloadContainers() entry");
  config_.at("containers").at("preload").const_list_for_each([this](auto value) {
    ModuleLoader::loadContainerFactory(store_, {"./", path_}, {
      {"typeName", value.at("typeName")}, {"load_paths", config_.at("containers").at("load_paths")}
    });
    if (value.hasKey("operations")) {
      value.at("operations").const_list_for_each([this, &value](auto& v) {
        ModuleLoader::loadContainerOperationFactory(store_, {"./", path_}, {
          {"typeName", v}, {"container_name", value.at("typeName")}, {"load_paths", config_.at("containers").hasKey("load_paths")}
        });
      });
    }
  });
  
  config_.at("containers").at("precreate").const_list_for_each([this](auto& value) {
    ObjectFactory::createContainer(store_, value);
  });

  logger::trace("Process::_preloadContainers() exit");
}

void Process::_preloadFSMs() {
  logger::trace("Process::_preloadFSMs() entry");
  config_.at("fsms").at("precreate").const_list_for_each([this](auto& value) {
    ObjectFactory::createFSM(store_, value);
  });
  logger::trace("Process::_preloadFSMs() exit");
}


void Process::_preloadExecutionContexts() {
  logger::trace("Process::_preloadExecutionContexts() entry");
  config_.at("ecs").at("preload").const_list_for_each([this](auto& value) {
    ModuleLoader::loadExecutionContextFactory(store_, {"./", path_}, {
      {"typeName", value}, {"load_paths", config_.at("ecs").at("load_paths")}
    });
  });
  
  config_.at("ecs").at("precreate").const_list_for_each([this](auto& value) {
    ObjectFactory::createExecutionContext(store_, value);
  });

  config_.at("ecs").at("bind").const_object_for_each([this](auto key, auto value) {
    value.const_list_for_each([this, &key](auto& v) {
      store()->executionContext(key)->bind(store()->operation(Value::string(v.at("fullName"))));
    });
  });
  
  logger::trace("Process::_preloadExecutionContexts() exit");
}

/**
 * 
 */
void Process::_preStartFSMs() {
  logger::trace("Process::_preStartFSMs() entry");
  
    config_.at("fsms").at("bind").at("operations").const_list_for_each([this](auto& bindInfo) {
      auto fsmInfo = bindInfo.at("fsm");
      auto opInfo = bindInfo.at("operation");
      store()->fsm(Value::string(fsmInfo.at("fullName")))->fsmState(Value::string(fsmInfo.at("state")))->bind(
        store()->operation(Value::string(opInfo.at("fullName"))),
        opInfo.at("argument")
      );
      /*
      if (opInfo.hasKey("argument")) {
        this->store()->getFSM(fsmInfo)->bindStateToOperation(fsmInfo.at("state").stringValue(), 
          this->store()->getAllOperation(opInfo), opInfo.at("argument")
        );
      } else {
        this->store()->getFSM(fsmInfo)->bindStateToOperation(fsmInfo.at("state").stringValue(), 
          this->store()->getAllOperation(opInfo)
        );
      }
      */
    });
    auto c = config_.at("fsms").at("bind");
    auto ecs = c.at("ecs");
    /*
    ecs.list_for_each([this](auto& value) {
      // TODO: ECへのバインド
      //store()->fsm(Value::string(value.at("fsm").at("fullName")))
      
      auto fsmInfo = value.at("fsm");
      auto ecInfo = value.at("ec");
      if (ecInfo.at("state").stringValue() == "started") {
        this->store()->getFSM(fsmInfo)->bindStateToECStart(fsmInfo.at("state").stringValue(), 
          this->store()->getExecutionContext(ecInfo)
        );
      } else if (ecInfo.at("state").stringValue() == "stopped") {
        this->store()->getFSM(fsmInfo)->bindStateToECStop(fsmInfo.at("state").stringValue(), 
          this->store()->getExecutionContext(ecInfo)
        );
      }
      
    });
    */
  //} catch (nerikiri::ValueTypeError& e) {
  //  logger::debug("Process::_preloadFSMs(). ValueTypeException:{}", e.what());
  //}

  logger::trace("Process::_preStartFSMs() exit");
}

void Process::_preStartExecutionContexts() {
  logger::trace("Process::_preStartExecutionContexts() entry");
  nerikiri::getListValue(config_, "ecs.start").const_list_for_each([this](const auto& value) {
    this->store()->executionContext(value.stringValue())->start();
  });
    /*
    auto c = config_.at("ecs").at("start");
    c.list_for_each([this](auto& value) {
      this->store()->getExecutionContext(value.stringValue())->start();
    });
    */
  logger::trace("Process::_preStartExecutionContexts() exit");
}

void Process::_preloadBrokers() {
  logger::trace("Process::_preloadBrokers() entry");
  config_.at("brokers").at("preload").const_list_for_each([this](auto& value) {
    ModuleLoader::loadBrokerFactory(store_, {"./", path_}, {
      {"typeName", value}, {"load_paths", config_.at("brokers").at("load_paths")}
    });
  });
  config_.at("brokers").at("precreate").const_list_for_each([this](auto& value) {
    ObjectFactory::createBroker(store_, value);
  });

  logger::trace("Process::_preloadBrokers() exit");
}

void Process::_preloadConnections() {
  logger::trace("Process::_preloadConnections() entry");
  config_.at("connections").const_list_for_each([this](auto& value) {
   // ConnectionBuilder::registerProviderConnection(store(), value);
    ConnectionBuilder::createConnection(store(), value);
  });
  logger::trace("Process::_preloadConnections() exit");
}


void Process::_preloadTopics() {
  logger::trace("Process::_preloadTopics() entry");
  try {
    auto operationCallback = [this](const Value& opInfo) -> void {
      logger::trace("Process::_preloadTopics(): operationCallback for opInfo={}", opInfo);
      getListValue(opInfo, "publish").const_list_for_each([this, &opInfo](auto v) {
       // ConnectionBuilder::registerTopicPublisher(store(), opInfo, ObjectFactory::createTopic(store_,
       //  {{"fullName", v.stringValue()}, {"defaultArg", { {"data", {}} }}} ));
      });
      /*
      opInfo.at("publish").const_list_for_each([this, &opInfo](auto v) {
        ConnectionBuilder::registerTopicPublisher(store(), opInfo, ObjectFactory::createTopic(store_,
         {{"fullName", v.stringValue()}, {"defaultArg", { {"data", {}} }}} ));
      });
      */
      getObjectValue(opInfo, "subscribe").const_object_for_each([this, &opInfo](auto key, auto v) {
        v.list_for_each([this, &opInfo, key](auto sv) {
         // ConnectionBuilder::registerTopicSubscriber(store(), opInfo, key, ObjectFactory::createTopic(store_,
         //  {{"fullName", sv.stringValue()}, {"defaultArg", { {"data", {}} }}} ));
        });
      });
      /*
      opInfo.at("subscribe").const_object_for_each([this, &opInfo](auto key, auto v) {
        v.list_for_each([this, &opInfo, key](auto sv) {
          ConnectionBuilder::registerTopicSubscriber(store(), opInfo, key, ObjectFactory::createTopic(store_,
           {{"fullName", sv.stringValue()}, {"defaultArg", { {"data", {}} }}} ));
        });
      });
      */
    };
    nerikiri::functional::for_each<std::shared_ptr<OperationAPI>>(store()->operations(), [operationCallback](auto op) {
      operationCallback(op->info());
    });
    
    //store()->getOperationInfos().const_list_for_each(operationCallback);
    //for(auto pc : store()->getContainers()) {
    //  for(auto opInfo : pc->getOperationInfos()) {
    //    operationCallback(opInfo);
    //  }
    //}

  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadTopics(). ValueTypeException:{}", e.what());
  } 
  logger::trace("Process::_preloadTopics() exit");

}

void Process::_preloadCallbacksOnStarted() {
  try {
    auto c = config_.at("callbacks");
    c.const_list_for_each([this](auto& value) {
      // TODO: コールバックね
      if (value.at("name").stringValue() == "on_started") {
        value.at("target").const_list_for_each([this](auto& v) {
          auto opName = v.at("fullName").stringValue();
          auto argument = v.at("argument");
          store()->operation(opName)->call(argument);
        });
      }
    });
  } catch (ValueTypeError& ex) {
    logger::error("Process::_preloadCallbacksOnStarted failed. Exception: {}", ex.what());
  }
}


Process& Process::addSystemEditor(SystemEditor_ptr&& se) {
  logger::trace("Process::addSystemEditor()");
  systemEditors_.emplace(se->name(), std::forward<SystemEditor_ptr>(se));
  return *this;
}

static auto start_broker(std::vector<std::shared_ptr<std::thread>>& threads, Process* process, std::shared_ptr<BrokerAPI> brk) {
  logger::trace("Creating a thread for broker {}", brk->info());
  std::promise<bool> prms;
  auto ftr = prms.get_future();
  threads.emplace_back(std::make_shared<std::thread>([brk, process](std::promise<bool> p) {
				     std::stringstream ss;
				     ss << std::this_thread::get_id();
				     logger::trace("A thread {} is going to run broker {}", ss.str(), str(brk->info()));
				     p.set_value(brk->run(process));
				     logger::trace("A thread {} finished", ss.str());
				   }, std::move(prms)));
  return ftr;
}

static std::future<bool> start_systemEditor(std::vector<std::shared_ptr<std::thread>>& threads, const SystemEditor_ptr& se) {
  logger::trace("Creating a thread for SystemEditor {}", se->name());
  std::promise<bool> prms;
  auto ftr = prms.get_future();
  threads.emplace_back(std::make_shared<std::thread>([&se](std::promise<bool> p) {
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

  setObjectState("starting");

  if (on_starting_) on_starting_(this);
  _preloadOperations();
  _preloadContainers();
  _preloadFSMs();
  _preloadExecutionContexts();
  _preloadBrokers();
  _preStartFSMs();
  _preStartExecutionContexts();
  auto broker_futures = nerikiri::functional::map<std::future<bool>, std::shared_ptr<BrokerAPI>>(store()->brokers(), [this](auto& brk) -> std::future<bool> {
								   return start_broker(this->threads_, this, brk);
								 });
  
  auto systemEditor_futures = nerikiri::map<std::future<bool>, std::string, SystemEditor_ptr>(this->systemEditors_,
											      [this](const auto& name, const auto& se) {
												return start_systemEditor(this->threads_, se); }
											      );

  for(auto& b : store_.brokers_) {
    while(true) {
      if (b->isNull()) break;

      if (b->isRunning()) { 
        break;
      }
      std::this_thread::sleep_for( std::chrono::milliseconds( 10 ));
    }
  }

  setObjectState("started");
  started_ = true;
  _preloadConnections();
  _preloadTopics();
  _preloadCallbacksOnStarted();
  if (on_started_) on_started_(this);


  // system("open -a 'Google Chrome' http://localhost:8080/process/fullInfo");
}
  
int32_t Process::wait() {
  logger::trace("Process::wait()");
  if (!wait_for(SIGNAL_INT)) {
    logger::error("Process::start method try to wait for SIGNAL_INT, but can not.");
    return -1;
  }
  return 0;
}

void Process::stop() {
  logger::trace("Process::shutdown()");
  setObjectState("stopping");
  nerikiri::foreach<std::shared_ptr<BrokerAPI> >(store_.brokers_, [this](auto& brk) {
			      brk->shutdown(this);
			    });
  nerikiri::foreach<std::string, SystemEditor_ptr>(systemEditors_, [this](auto& name, auto& se) {
				      se->shutdown();
				    });
  
  logger::trace(" - joining...");
  for(auto& t : this->threads_) {
    t->join();
  }
  started_ = false;
  setObjectState("stopped");
  logger::trace(" - joined");
}


int32_t Process::start() {
  logger::trace("Process::start()");
  startAsync();
  if (wait() < 0) return -1;
  stop();
  return 0;
}


Value Process::getCallbacks() const {
  logger::trace("Process::getCallbacks()");
  auto v = this->config_.at("callbacks");
  if (v.isError()) return Value({});
  return v;
}


Value Process::fullInfo() const {
  auto inf = info();
  inf["operations"] = functional::map<Value, std::shared_ptr<OperationAPI>>(store()->operations(), [](auto op) { return op->info(); });
  inf["operationFactories"] = functional::map<Value, std::shared_ptr<OperationFactoryAPI>>(store()->operationFactories(), [](auto op) { return op->info(); });
  inf["brokers"] = functional::map<Value, std::shared_ptr<BrokerAPI>>(store()->brokers(), [](auto o) { return o->fullInfo(); });  
  inf["brokerFactories"] = functional::map<Value, std::shared_ptr<BrokerFactoryAPI>>(store()->brokerFactories(), [](auto op) { return op->info(); });

  return inf;
}