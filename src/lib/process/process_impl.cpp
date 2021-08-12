#include <sstream>
#include <iostream>
#include <future>

#include <juiz/utils/os.h>
#include <juiz/utils/signal.h>
#include <juiz/process.h>

#include "../argparse.h"
#include "../objectfactory.h"

#include "process_impl.h"
#include "processconfigparser.h"
#include "moduleloader.h"
#include "process_builder.h"

#include "../fsm/fsm_container.h"
#include "../ec/ec_container.h"

using namespace juiz;
using namespace juiz::logger;


std::shared_ptr<ProcessAPI> juiz::process(const std::string& name) {
  return std::make_shared<ProcessImpl>(name);
}

std::shared_ptr<ProcessAPI> juiz::process(const int argc, const char** argv) {
  return std::make_shared<ProcessImpl>(argc, argv);
}

std::shared_ptr<ProcessAPI> juiz::process(const std::string& name, const Value& config) {
  return std::make_shared<ProcessImpl>(name, config);
}

std::shared_ptr<ProcessAPI> juiz::process(const std::string& name, const std::string& jsonStr) {
  return std::make_shared<ProcessImpl>(name, jsonStr);
}


  
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

ProcessImpl::ProcessImpl(const std::string& name) : ProcessAPI("Process", "Process", name), store_(this), config_(defaultProcessConfig), started_(false), env_dictionary_(env_dictionary_default) {
  std::string fullpath = name;
  if (fullpath.find("/") != 0) {
    fullpath = juiz::getCwd() + "/" + fullpath;
  }

  std::string path = fullpath.substr(0, fullpath.rfind("/")+1);
  auto cf = coreBrokerFactory(this, "coreBroker0");
  coreBroker_ = cf->createProxy({});
  //coreBroker_ = std::make_shared<CoreBroker>(this, "coreBroker0");
  try {
    store_.addBrokerFactory(cf);
    store_.add<TopicFactoryAPI>(topicFactory());
    // store_.addFSMFactory(fsmFactory("fsmFactory"));

    setupFSMContainer(*this->store());
    setupECContainer(*this->store());
    setExecutablePath(getExecutablePath(name));

    env_dictionary_["${ExecutableDirectory}"] = path_.substr(0, path_.rfind('/'));

  } catch (std::exception & ex) {
    logger::error("Process::Process failed. Exception: {}", ex.what());
  }
}

ProcessImpl::ProcessImpl(const int argc, const char** argv) : ProcessImpl(argv[0]) {
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

ProcessImpl::ProcessImpl(const std::string& name, const Value& config) : ProcessImpl(name) {
  config_ = juiz::merge(config_, config);  
  _setupLogger();
  logger::info("Process::Process(\"{}\")", name);
  logger::info("Process::Process() - ExecutablePath = {}", path_);
  logger::info("ExecutableDirectory = {}", env_dictionary_["${ExecutableDirectory}"]);
}

ProcessImpl::ProcessImpl(const std::string& name, const std::string& jsonStr): ProcessImpl(name) {
  config_ = merge(config_, ProcessConfigParser::parseConfig(jsonStr));
  _setupLogger();
  logger::info("Process::Process(\"{}\")", name);
}

ProcessImpl::~ProcessImpl() {
  logger::trace("Process::~Process()");
  if (started_) {
    stop();
  }
}

void ProcessImpl::_setupLogger() {
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

void ProcessImpl::parseConfigFile(const std::string& filepath) {
  logger::trace("Process::parseConfigFile({}) entry", filepath);

  /// まず読み込んでみてロガーの設定を確認します
  auto fp = fopen(filepath.c_str(), "r");
  if (fp == nullptr) {
    logger::warn("ProcessConfigParser::parseConfig failed. File pointer is NULL.");
  }
  auto v = juiz::json::toValue(fp);
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
    env_dictionary_["${ProjectDirectory}"] = juiz::getCwd() + "/";
  } else {
    env_dictionary_["${ProjectDirectory}"] = filepath.substr(0, filepath.rfind("/")+1);
  }
  /// ここで環境変数の辞書の適用
  config_ = replaceAndCopy(config_, env_dictionary_);
  logger::info("Process::parseConfigFile -> {}", config_);

  logger::trace("Process::parseConfigFile({}) exit", filepath);
}

ProcessImpl& ProcessImpl::addSystemEditor(SystemEditor_ptr&& se) {
  logger::trace("Process::addSystemEditor() called");
  systemEditors_.emplace(se->name(), std::forward<SystemEditor_ptr>(se));
  logger::trace("Process::addSystemEditor() exit");
  return *this;
}

static auto start_broker(std::vector<std::shared_ptr<std::thread>>& threads, const std::shared_ptr<ClientProxyAPI>& broker, std::shared_ptr<BrokerAPI> brk) {
  logger::trace("Creating a thread for broker {}", brk->info());
  std::promise<bool> prms;
  auto ftr = prms.get_future();
  threads.emplace_back(std::make_shared<std::thread>([brk, broker](std::promise<bool> p) {
				     std::stringstream ss;
				     ss << std::this_thread::get_id();
				     logger::trace("A thread {} is going to run broker {}", ss.str(), str(brk->info()));
				     p.set_value(brk->run(broker));
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

void ProcessImpl::startAsync() {
  logger::trace("Process::startAsync()");

  setObjectState("starting");

  if (on_starting_) on_starting_(this);
  
  ProcessBuilder::preloadOperations(store_, config_, path_);
  ProcessBuilder::preloadContainers(store_, config_, env_dictionary_["${ProjectDirectory}"]);  
  ProcessBuilder::preloadFSMs(store_, config_, path_);
  ProcessBuilder::preloadExecutionContexts(store_, config_, path_);
  ProcessBuilder::preloadBrokers(store_, config_, path_);
  ProcessBuilder::preStartFSMs(store_, config_, path_);
  ProcessBuilder::preStartExecutionContexts(store_, config_, path_);

  auto broker_futures = juiz::functional::map<std::future<bool>, std::shared_ptr<BrokerAPI>>(store()->brokers(), [this](auto& brk) -> std::future<bool> {
								   return start_broker(this->threads_, coreBroker_, brk);
								 });
  
  auto systemEditor_futures = juiz::map<std::future<bool>, std::string, SystemEditor_ptr>(this->systemEditors_,
											      [this](const auto& name, const auto& se) {
												return start_systemEditor(this->threads_, se); }
											      );

  /// Check brokers are started.
  for(auto& b : store_.brokers()) {
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
  
  ProcessBuilder::preloadConnections(store_, config_, path_);
  ProcessBuilder::preloadTopics(store_, coreBroker_, config_, path_);
  ProcessBuilder::preloadCallbacksOnStarted(store_, config_, path_);

  if (on_started_) on_started_(this);
}
  
int32_t ProcessImpl::wait() {
  logger::trace("Process::wait()");
  if (!wait_for(SIGNAL_INT)) {
    logger::error("Process::start method try to wait for SIGNAL_INT, but can not.");
    return -1;
  }
  return 0;
}

void ProcessImpl::stop() {
  logger::trace("Process::shutdown()");
  setObjectState("stopping");
  juiz::foreach<std::shared_ptr<BrokerAPI> >(store_.brokers(), [this](auto& brk) {
			      brk->shutdown(coreBroker_);
			    });
  juiz::foreach<std::string, SystemEditor_ptr>(systemEditors_, [this](auto& name, auto& se) {
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


int32_t ProcessImpl::start() {
  logger::trace("Process::start()");
  startAsync();
  if (wait() < 0) return -1;
  stop();
  return 0;
}


Value ProcessImpl::getCallbacks() const {
  logger::trace("Process::getCallbacks()");
  auto v = this->config_.at("callbacks");
  if (v.isError()) return Value({});
  return v;
}


Value ProcessImpl::fullInfo() const {
  auto inf = info();
  inf["operations"] = functional::map<Value, std::shared_ptr<OperationAPI>>(store()->list<OperationAPI>(), [](auto op) { return op->fullInfo(); });
  inf["operationFactories"] = functional::map<Value, std::shared_ptr<OperationFactoryAPI>>(store()->list<OperationFactoryAPI>(), [](auto op) { return op->info(); });
  inf["brokers"] = functional::map<Value, std::shared_ptr<BrokerAPI>>(store()->brokers(), [](auto o) { return o->fullInfo(); });  
  inf["brokerFactories"] = functional::map<Value, std::shared_ptr<BrokerFactoryAPI>>(store()->brokerFactories(), [](auto op) { return op->info(); });
  inf["containers"] = functional::map<Value, std::shared_ptr<ContainerAPI>>(store()->list<ContainerAPI>(), [](auto c) { return c->fullInfo(); });  
  //inf["fsms"] = functional::map<Value, std::shared_ptr<FSMAPI>>(store()->fsms(), [](auto c) { return c->info(); });  
  ///inf["ecs"] = functional::map<Value, std::shared_ptr<ExecutionContextAPI>>(store()->executionContexts(), [](auto c) { return c->info(); });  
  return inf;
}



