#include <sstream>
#include <iostream>
#include <future>
#include <filesystem>

#include <juiz/utils/os.h>
#include <juiz/utils/signal.h>
#include <juiz/process.h>
#include <juiz/utils/json.h>
#include <juiz/utils/yaml.h>

#include <juiz/utils/argparse.h>
#include "../objectfactory.h"

#include "process_impl.h"
#include "processconfigparser.h"
#include "moduleloader.h"
#include "process_builder.h"

#include "../fsm/fsm_container.h"
#include "../ec/ec_container.h"
#include "../anchor/anchor_container.h"

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
    {"logLevel", "ALL"}
  }},
  {"operations", {
    {"precreate", Value::list()},
    {"preload", Value::list()}
  }},
  {"containers", {
    {"precreate", Value::list()},
    {"preload", Value::list()},
    {"transformation", Value::list()}
  }},
  {"anchors", Value::list()},
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

std::filesystem::path ProcessImpl::_guessFullPath(const std::string& command) const {
  std::filesystem::path p(command);
  if (!p.is_absolute()) {
    p = std::filesystem::current_path() / p;
  }
  return p;
}

/**
 * @brief Construct a new Process Impl:: Process Impl object 
 * 
 * デフォルトコンストラクタ
 * 
 * @param name 
 */
ProcessImpl::ProcessImpl(const std::string& name) : ProcessAPI("Process", "Process", name),
  store_(this), config_(defaultProcessConfig), 
  started_(false), 
  env_dictionary_(env_dictionary_default), 
  fullpath_(_guessFullPath(name))
{
  try {
    auto cf = coreBrokerFactory(this);
    coreBroker_ = cf->createProxy({});
    store_.addBrokerFactory(cf);
    store_.add<TopicFactoryAPI>(topicFactory());
    setupFSMContainer(*this->store());
    setupECContainer(*this->store());
    setupAnchorContainer(*this->store());
    env_dictionary_["${ExecutableDirectory}"] = fullpath_.string();
  } catch (std::exception & ex) {
    logger::error("Process::Process failed. Exception: {}", ex.what());
  }
}

ProcessImpl::ProcessImpl(const int argc, const char** argv) : ProcessImpl(argv[0]) {
  ArgParser parser;
  parser.option<std::string>("-ll", "--loglevel", "Log Level", false, "INFO");
  auto options = parser.parse(argc, argv);
  logger::setLogLevel(options.get<std::string>("loglevel"));

  logger::trace("-------------------- Starting Process -------------------");
  if (options.unknown_args.size() > 0) {
    _parseConfigFile(options.unknown_args[0]);
  }
  _setupLogger(config_["logger"]);

  logger::info("ProcessImpl::Process(argv[0]=\"{}\")", argv[0]);
  logger::info("ProcessImpl::Process() - ExecutablePath = {}", this->fullpath_);
  logger::info("ExecutableDirectory = {}", env_dictionary_["${ExecutableDirectory}"]);
}

ProcessImpl::ProcessImpl(const std::string& name, const Value& config) : ProcessImpl(name) {
  config_ = juiz::merge(config_, config);  
  _setupLogger(config_["logger"]);
  logger::info("ProcessImpl::Process(\"{}\")", name);
  logger::info("ProcessImpl::Process() - ExecutablePath = {}", this->fullpath_);
  logger::info("ExecutableDirectory = {}", env_dictionary_["${ExecutableDirectory}"]);
}

ProcessImpl::ProcessImpl(const std::string& name, const std::string& jsonStr): ProcessImpl(name) {
  config_ = merge(config_, (juiz::json::toValue(jsonStr)));
  _setupLogger(config_["logger"]);
  logger::info("ProcessImpl::Process(\"{}\")", name);
}

ProcessImpl::~ProcessImpl() {
  logger::trace("ProcessImpl::~Process() called.");
  if (started_) {
    stop();
  }
}

void ProcessImpl::_setupLogger(const Value& loggerConf) const {
  if (loggerConf.isError()) {
    logger::error("ProcessImpl::_setupLogger() failed. Argument Value is error.");
    return;
  }
  logger::trace("ProcessImpl::_setupLogger() called.");
  logger::setLogLevel(getStringValue(loggerConf["logLevel"], "INFO"));
  logger::setLogFileName(getStringValue(loggerConf["logFile"], ""));
  logger::initLogger();
  logger::trace("ProcessImpl::_setupLogger() exit.");
}


namespace {

  Value loadNKProj(const std::filesystem::path& _path) {
    return juiz::json::toValue(fopen(_path.string().c_str(), "r"));
  }

  Value loadJProj(const std::filesystem::path& _path) {
    std::ifstream src_f(_path);
    return juiz::yaml::toValue(src_f);
  }

  Value loadConfigFile(const std::filesystem::path& _path) {
    if (_path.extension() == ".nkproj") {
      return loadNKProj(_path);
    } else if (_path.extension() == ".jproj") {
      return loadJProj(_path);
    }
  }
}
/**
 * @brief この部分でコンフィグファイルを読み込み，Valueデータに変換し，まずロガーを設定してからincludeをマージします．
 * 
 * @param filepath 
 */
void ProcessImpl::_parseConfigFile(const std::filesystem::path& filepath) {
  logger::trace("ProcessImpl::parseConfigFile({}) entry", filepath);
  /// ここでプロジェクトを読み込む
  config_ = merge(config_, ProcessConfigParser::parseProjectFile(filepath));
  /// ここで環境変数の辞書の設定
  env_dictionary_["${ProjectDirectory}"] = filepath.parent_path();
  /// ここで環境変数の辞書の適用
  config_ = replaceAndCopy(config_, env_dictionary_);
  if (config_.isError()) {
    logger::error("ProcessImpl::parseConfigFile({}) failed. Configuration Error.", filepath);
  }
  logger::trace("---- configuration -----");
  logger::trace("{}", juiz::yaml::toYAMLString(config_, false));
  logger::trace("------------------------");
  logger::trace("Process::parseConfigFile({}) exit", filepath);
}

ProcessImpl& ProcessImpl::addSystemEditor(SystemEditor_ptr&& se) {
  logger::trace("ProcessImpl::addSystemEditor() called");
  systemEditors_.emplace(se->name(), std::forward<SystemEditor_ptr>(se));
  logger::trace("ProcessImpl::addSystemEditor() exit");
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
  logger::trace("ProcessImpl::startAsync() called");

  setObjectState("starting");

  if (on_starting_) on_starting_(this);
  
  ProcessBuilder::preloadOperations(store_, config_, fullpath_);
  ProcessBuilder::preloadContainers(store_, config_, env_dictionary_["${ProjectDirectory}"]);  
  ProcessBuilder::preloadFSMs(store_, config_, fullpath_);
  ProcessBuilder::preloadExecutionContexts(store_, config_, fullpath_);
  ProcessBuilder::preloadAnchors(store_, config_, fullpath_);
  ProcessBuilder::preloadBrokers(store_, config_, fullpath_);
  ProcessBuilder::preStartFSMs(store_, config_, fullpath_);
  ProcessBuilder::preStartExecutionContexts(store_, config_, fullpath_);

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
  
  ProcessBuilder::preloadConnections(store_, config_, fullpath_);
  ProcessBuilder::preloadTopics(store_, coreBroker_, config_, fullpath_);
  ProcessBuilder::preloadCallbacksOnStarted(store_, config_, fullpath_);

  if (on_started_) on_started_(this);
}
  
int32_t ProcessImpl::wait() {
  logger::trace("ProcessImpl::wait() called");
  if (!wait_for(SIGNAL_INT)) {
    logger::error("Process::start method try to wait for SIGNAL_INT, but can not.");
    return -1;
  }
  return 0;
}

void ProcessImpl::stop() {
  logger::trace("ProcessImpl::stop() called");
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
  logger::trace("ProcessImpl::start() called");
  startAsync();
  if (wait() < 0) return -1;
  stop();
  return 0;
}


Value ProcessImpl::getCallbacks() const {
  logger::trace("ProcessImpl::getCallbacks() called");
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



