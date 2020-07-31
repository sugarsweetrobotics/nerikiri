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
    {"preload", Value::list()}
  }},
  {"containers", {
    {"preload", Value::object()}
  }},
  {"containerOperations", {
    {"preload", Value::list()}
  }},
  {"ecs", {
    {"preload", Value::list()}
  }},
  {"brokers", {
    {"preload", Value::list()}
  }},

  {"callbacks", Value::list()}
});

Process Process::null("");

Process::Process(const std::string& name) : Object({{"typeName", "Process"}, {"instanceName", name}, {"fullName", name}}), store_(this), config_(defaultProcessConfig), started_(false), env_dictionary_(env_dictionary_default) {
  std::string fullpath = name;
  if (fullpath.find("/") != 0) {
    fullpath = nerikiri::getCwd() + "/" + fullpath;
  }

  std::string path = fullpath.substr(0, fullpath.rfind("/")+1);
  coreBroker_ = std::make_shared<CoreBroker>(this, Value({{"typeName", "CoreBroker"}, {"instanceName", "coreBroker0"}, {"fullName", "coreBroker0"}}));
  try {
    store_.addBrokerFactory(std::make_shared<CoreBrokerFactory>(coreBroker_));
    store_.addTopicFactory(std::make_shared<TopicFactory>());
    store_.addFSMFactory(std::make_shared<FSMFactory>());
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
  }
}

void Process::parseConfigFile(const std::string& filepath) {
  logger::trace("Process::parseConfigFile({})", filepath);

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
}

void Process::_preloadOperations() {
  logger::trace("Process::_preloadOperations()");
  try {
    auto c = config_.at("operations").at("preload");
    c.list_for_each([this](auto& value) {
      Value v = {{"typeName", value}};
      if (config_.at("operations").hasKey("load_paths")) {
        v["load_paths"] = config_.at("operations").at("load_paths");
      }
      ModuleLoader::loadOperationFactory(store_, {"./", path_}, v);
    });
  
  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadOperations(). ValueTypeException:{}", e.what());
  } 
  
  try {
  auto c = config_.at("operations").at("precreate");
  c.list_for_each([this](auto& oinfo) {
     // createOperation(oinfo);  
    ObjectFactory::createOperation(store_, oinfo);
  });
  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadOperations(). ValueTypeException:{}", e.what());
  }
}


void Process::_preloadContainers() {
  logger::trace("Process::_preloadContainers()");
  try {
    auto c = config_.at("containers").at("preload");
    c.object_for_each([this](auto& key, auto& value) {
      Value v = {{"typeName", key}};
      if (config_.at("containers").hasKey("load_paths")) {
        v["load_paths"] = config_.at("containers").at("load_paths");
      }
      ModuleLoader::loadContainerFactory(store_, {"./", path_}, v);
      if (v.isError()) {
        logger::error("Process::_preloadContainers({}) failed. {}", key, v.getErrorMessage());
      }
      value.list_for_each([this, &key](auto& v) {
        Value vv = {{"typeName", v}};
        if (config_.at("containers").hasKey("load_paths")) {
          vv["load_paths"] = config_.at("containers").at("load_paths");
        }
        vv["container_name"] = key;
        ModuleLoader::loadContainerOperationFactory(store_, {"./", path_}, vv);
      });
    });
  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadContainers(). ValueTypeException:{}", e.what());
  }

  try {
    auto c = config_.at("containers").at("precreate");
    c.list_for_each([this](auto& value) {
      //auto cinfo = createContainer(value);
      auto cinfo = ObjectFactory::createContainer(store_, value);
      if (cinfo.isError()) {
        logger::error("Porcess::_preloadContainers({}) failed. createContainer error: ({})", value, str(cinfo));
      }
    });
  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadContainers(). ValueTypeException:{}", e.what());
  }
}

void Process::_preloadExecutionContexts() {
  logger::trace("Process::_preloadExecutionContexts()");
  try {
    auto c = config_.at("ecs").at("preload");
    c.list_for_each([this](auto& value) {
      Value v = {{"typeName", value}};
      if (config_.at("ecs").hasKey("load_paths")) {
        v["load_paths"] = config_.at("ecs").at("load_paths");
      }
      ModuleLoader::loadExecutionContextFactory(store_, {"./", path_}, v);
    });
  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadExecutionContexts(). ValueTypeException:{}", e.what());
  }

  try {
    auto c = config_.at("ecs").at("precreate");
    c.list_for_each([this](auto& value) {
      //auto cinfo = createExecutionContext(value);
      auto cinfo = ObjectFactory::createExecutionContext(store_, value);
    });
  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadExecutionContexts(). ValueTypeException:{}", e.what());
  }

  try {
    auto c = config_.at("ecs").at("bind");
    c.object_for_each([this](auto& key, auto& value) {
      value.list_for_each([this, key](auto& v) {
        auto broker = store()->getBrokerFactory({{"typeName", v.at("broker").stringValue()}})->createProxy(v.at("broker"));
        auto fullName = v.at("fullName").stringValue();
        store()->getExecutionContext(key)->bind(fullName, broker);
      });
    });
  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadExecutionContexts(). ValueTypeException:{}", e.what());
  }
}

void Process::_preloadFSMs() {
  logger::trace("Process::_preloadFSMs()");
  

  try {
    auto c = config_.at("fsms").at("precreate");
    c.list_for_each([this](auto& value) {
      auto cinfo = ObjectFactory::createFSM(store_, value);
    });
  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadFSMs(). ValueTypeException:{}", e.what());
  }
}


void Process::_preStartExecutionContexts() {

  logger::trace("Process::_preStartExecutionContexts()");
  try {
    auto c = config_.at("ecs").at("start");
    c.list_for_each([this](auto& value) {
      this->store()->getExecutionContext(value.stringValue())->start();
    });
  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadOperations(). ValueTypeException:{}", e.what());
  }
}

void Process::_preloadBrokers() {
  try {
    auto c = config_.at("brokers").at("preload");
    c.list_for_each([this](auto& value) {
      Value v = {{"typeName", value}};
      if (config_.at("brokers").hasKey("load_paths")) {
        v["load_paths"] = config_.at("brokers").at("load_paths");
      }
      ModuleLoader::loadBrokerFactory(store_, {"./", path_}, v);
    });
  } catch (ValueTypeError& ex) {
    logger::error("Process::_preloadBrokers in preload stage failed. ValueTypeError: {}", ex.what());
  }

  try {
    auto c = config_.at("brokers").at("precreate");
    c.list_for_each([this](auto& value) {
      //createBroker(value);
      ObjectFactory::createBroker(store_, value);
    });
  } catch (ValueTypeError& ex) {
    logger::error("Process::_preloadBrokers failed in precreate stage failed. Exception: {}", ex.what());
  }
}

void Process::_preloadConnections() {
  try {
    auto c = config_.at("connections");
    c.list_for_each([this](auto& value) {
      ConnectionBuilder::registerProviderConnection(store(), value);
      //this->registerProviderConnection(value);
    });
  } catch (ValueTypeError& ex) {
    logger::error("Process::_preloadConnections failed. Exception: {}", ex.what());
  }
}


void Process::_preloadTopics() {
  try {
    auto operationCallback = [this](auto& opInfo) {
      opInfo.at("publish").list_for_each([this, &opInfo](auto v) {
        ConnectionBuilder::registerTopicPublisher(store(), opInfo, ObjectFactory::createTopic(store_,
         {{"fullName", v.stringValue()}, {"defaultArg", { {"data", {}} }}} ));
      });
      opInfo.at("subscribe").object_for_each([this, &opInfo](auto key, auto v) {
        v.list_for_each([this, &opInfo, key](auto sv) {
          ConnectionBuilder::registerTopicSubscriber(store(), opInfo, key, ObjectFactory::createTopic(store_,
           {{"fullName", sv.stringValue()}, {"defaultArg", { {"data", {}} }}} ));
        });
      });
    };

    store()->getOperationInfos().list_for_each(operationCallback);
    for(auto pc : store()->getContainers()) {
      for(auto opInfo : pc->getOperationInfos()) {
        operationCallback(opInfo);
      }
    }

  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadTopics(). ValueTypeException:{}", e.what());
  } 
}

void Process::_preloadCallbacksOnStarted() {
  try {
    auto c = config_.at("callbacks");
    c.list_for_each([this](auto& value) {
      // TODO: コールバックね
      if (value.at("name").stringValue() == "on_started") {
        value.at("target").list_for_each([this](auto& v) {
          auto opName = v.at("fullName").stringValue();
          auto argument = v.at("argument");
          store()->getAllOperation(opName)->call(argument);
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

static auto start_broker(std::vector<std::shared_ptr<std::thread>>& threads, Process* process, std::shared_ptr<Broker> brk) {
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

  setState("starting");

  if (on_starting_) on_starting_(this);
  _preloadOperations();
  _preloadContainers();
  _preloadFSMs();
  _preloadExecutionContexts();
  _preloadBrokers();
  _preStartExecutionContexts();
  auto broker_futures = nerikiri::map<std::future<bool>, std::shared_ptr<Broker>>(store_.brokers_, [this](auto& brk) -> std::future<bool> {
								   return start_broker(this->threads_, this, brk);
								 });
  
  auto systemEditor_futures = nerikiri::map<std::future<bool>, std::string, SystemEditor_ptr>(this->systemEditors_,
											      [this](const auto& name, const auto& se) {
												return start_systemEditor(this->threads_, se); }
											      );

  for(auto& b : store_.brokers_) {
    while(true) {
      if (b->isRunning()) { 
        break;
      }
      std::this_thread::sleep_for( std::chrono::milliseconds( 10 ));
    }
  }

  setState("started");
  started_ = true;
  _preloadConnections();
  _preloadTopics();
  _preloadCallbacksOnStarted();
  if (on_started_) on_started_(this);
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
  setState("stopping");
  nerikiri::foreach<std::shared_ptr<Broker> >(store_.brokers_, [this](auto& brk) {
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
  setState("stopped");
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
