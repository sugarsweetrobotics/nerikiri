#include <sstream>
#include "nerikiri/nerikiri.h"
#include "nerikiri/util/functional.h"
#include "nerikiri/util/logger.h"
#include "nerikiri/util/signal.h"
#include "nerikiri/util/naming.h"
#include "nerikiri/util/argparse.h"
#include "nerikiri/util/os.h"

#include "nerikiri/brokers/broker.h"
#include "nerikiri/moduleloader.h"
#include "nerikiri/objectfactory.h"
#include "nerikiri/connectionbuilder.h"

#include "nerikiri/process.h"
#include "nerikiri/processconfigparser.h"

#include <iostream>

using namespace nerikiri;
using namespace nerikiri::logger;

//std::shared_ptr<Broker> Broker::null = std::make_shared<Broker>();;

std::string replaceAll(std::string s, const std::string& p, const std::string& m) {
    auto pos = s.find(p);
    int toLen = m.length();
 
    if (p.empty()) {
        return s;
    }
 
    while ((pos = s.find(p, pos)) != std::string::npos) {
        s.replace(pos, p.length(), m);
        pos += toLen;
    }
    return s;
}

static std::string stringReplace(std::string s, const std::map<std::string, std::string>& dictionary) {
  nerikiri::foreach<std::string, std::string>(dictionary, [&s](const std::string& k, const std::string& v) {
    s = replaceAll(s, k, v);
  });
  return s;
}

static Value replaceAndCopy(const Value& value, const std::map<std::string, std::string>& dictionary) {
  if (value.isObjectValue()) {
    Value v({});
    value.object_for_each([&v, &dictionary](auto& key, auto& cvalue) {
      v[key] = replaceAndCopy(cvalue, dictionary);
    });
    return v;
  }
  if (value.isListValue()) {
    Value v = Value::list();
    value.list_for_each([&v, &dictionary](auto& cvalue) {
      v.push_back(replaceAndCopy(cvalue, dictionary));
    });
    return v;
  }
  if (value.isStringValue()) {
    return stringReplace(value.stringValue(), dictionary);
  }

  return value;
}

std::map<std::string, std::string> env_dictionary_default{

};

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
  /*
    { {"name", Value("on_started")},
      {"target", Value::list()} },
    { {"name", Value("on_stopped")}, 
      {"target", Value::list()} }
  }}*/
});

Process Process::null("");

Process::Process(const std::string& name) : Object({{"name", "Process"}, {"instanceName", name}}), store_(this), config_(defaultProcessConfig), started_(false), env_dictionary_(env_dictionary_default) {
  std::string fullpath = name;
  if (fullpath.find("/") != 0) {
    fullpath = nerikiri::getCwd() + fullpath;
  }

  std::string path = fullpath.substr(0, fullpath.rfind("/")+1);
  coreBroker_ = std::make_shared<CoreBroker>(this, Value({{"name", "CoreBroker"}, {"instanceName", "coreBroker0"}}));
  setExecutablePath(path);
}

Process::Process(const int argc, const char** argv) : Process(argv[0]) {
  ArgParser parser;
  //parser.option<std::string>("-f", "--file", "Setting file path", false, "nk.json");




  auto options = parser.parse(argc, argv);
  if (options.unknown_args.size() > 0) {
    parseConfigFile(options.unknown_args[0]);
  }
  _setupLogger();
  logger::info("Process::Process(\"{}\")", argv[0]);
}

Process::Process(const std::string& name, const Value& config) : Process(name) {
  config_ = nerikiri::merge(config_, config);  
  _setupLogger();
  logger::info("Process::Process(\"{}\")", name);
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
  /// ここで環境変数の辞書の設定
  std::string fullpath = filepath;
  if (fullpath.find("/") != 0) {
    fullpath = nerikiri::getCwd();
  } else {
    fullpath = fullpath.substr(0, fullpath.rfind("/")+1);
  }
  env_dictionary_["${ProjectDirectory}"] = fullpath;


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
    config_ = merge(config_, ProcessConfigParser::parseConfig(fp));
  } else {
    logger::info("Process::parseConfigFile. Can not open file ({})", filepath);
  }

  /// ここで環境変数の辞書の適用
  config_ = replaceAndCopy(config_, env_dictionary_);

  logger::info("Process::parseConfigFile -> {}", config_);
}

void Process::_preloadOperations() {
  try {
    auto c = config_.at("operations").at("preload");
    c.list_for_each([this](auto& value) {
      Value v = {{"name", value}};
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
  try {
  auto c = config_.at("containers").at("preload");
  c.object_for_each([this](auto& key, auto& value) {
    Value v = {{"name", key}};
    if (config_.at("containers").hasKey("load_paths")) {
      v["load_paths"] = config_.at("containers").at("load_paths");
    }
    ModuleLoader::loadContainerFactory(store_, {"./", path_}, v);
    if (v.isError()) {
      logger::error("Process::_preloadContainers({}) failed. {}", key, v.getErrorMessage());
    }
    value.list_for_each([this, &key](auto& v) {
      Value vv = {{"name", v}};
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
  try {
    auto c = config_.at("ecs").at("preload");
    c.list_for_each([this](auto& value) {
      Value v = {{"name", value}};
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
        // this->bindECtoOperation(key, {{"instanceName", v.stringValue()}});      
        store()->getExecutionContext({{"instanceName", key}})->bind(store()->getOperation({{"instanceName", v.stringValue()}}));
      });
    });
  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadExecutionContexts(). ValueTypeException:{}", e.what());
  }
}


void Process::_preStartExecutionContexts() {
  try {
  auto c = config_.at("ecs").at("start");
  c.list_for_each([this](auto& value) {
    this->store()->getExecutionContext({{"instanceName", value}})->start();
  });
  } catch (nerikiri::ValueTypeError& e) {
    logger::debug("Process::_preloadOperations(). ValueTypeException:{}", e.what());
  }
}

void Process::_preloadBrokers() {
  try {
    auto c = config_.at("brokers").at("preload");
    c.list_for_each([this](auto& value) {
      ModuleLoader::loadBrokerFactory(store_, {"./", path_}, {{"name", value}});
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

void Process::_preloadCallbacksOnStarted() {
  try {
    auto c = config_.at("callbacks");
    c.list_for_each([this](auto& value) {
      // TODO: コールバックね
      if (value.at("name").stringValue() == "on_started") {
        value.at("target").list_for_each([this](auto& v) {
          auto opName = v.at("name").stringValue();
          auto argument = v.at("argument");
          store()->getOperation({{"instanceName", opName}})->call(argument);
        });
      }
    });
  } catch (ValueTypeError& ex) {
    logger::error("Process::_preloadConnections failed. Exception: {}", ex.what());
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
