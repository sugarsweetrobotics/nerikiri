#include <sstream>
#include "nerikiri/nerikiri.h"
#include "nerikiri/functional.h"
#include "nerikiri/process.h"
#include "nerikiri/logger.h"
#include "nerikiri/signal.h"

using namespace nerikiri;
using namespace nerikiri::logger;

Process::Process(const std::string& name) : info_(name)
{
  setLogLevel(TRACE);
  trace("Process::Process()");
}

Process::~Process() {
  trace("Process::~Process()");
}

Process& Process::addOperation(Operation&& op) {
  if (getOperationByName(info().at("name").stringValue()).isNull()) {
    operations_.emplace_back(std::move(op));
  } 
  error("Process::addOperation({}) Error. Process already has the same name operation", info().at("name").stringValue());
  return *this;
}

Process& Process::addOperation(const Operation& op) {
  operations_.push_back(op);
  return *this;
}

Process& Process::addSystemEditor(SystemEditor_ptr&& se) {
  trace("Process::addSystemEditor()");
  systemEditors_.emplace(se->name(), std::forward<SystemEditor_ptr>(se));
  return *this;
}

Process& Process::addBroker(Broker_ptr&& brk) {
  trace("Process::addBroker()");
  brk->setProcess(Process_ptr(this));
  //brokerDictionary_.add(std::forward<Broker_ptr>(brk));
  brokers_.emplace_back(std::forward<Broker_ptr>(brk));
  return *this;
}

static auto start_broker(std::vector<std::thread>& threads, Broker_ptr& brk) {
  logger::trace("Creating a thread for broker {}", str(brk->info()));
  std::promise<bool> prms;
  auto ftr = prms.get_future();
  threads.emplace_back(std::thread([&brk](std::promise<bool> p) {
				     std::stringstream ss;
				     ss << std::this_thread::get_id();
				     logger::trace("A thread {} is going to run broker {}", ss.str(), str(brk->info()));
				     p.set_value(brk->run());
				     logger::trace("A thread {} finished", ss.str());
				   }, std::move(prms)));
  return ftr;
}

static std::future<bool> start_systemEditor(std::vector<std::thread>& threads, SystemEditor_ptr& se) {
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
								   return start_broker(this->threads_, brk);
								 });
  
  auto systemEditor_futures = nerikiri::map<std::future<bool>, std::string, SystemEditor_ptr>(this->systemEditors_,
											      [this](auto& name, auto& se) {
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
  nerikiri::foreach<Broker_ptr>(brokers_, [](auto& brk) {
			      brk->shutdown();
			    });
  nerikiri::foreach<std::string, SystemEditor_ptr>(systemEditors_, [](auto& name, auto& se) {
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
  startAsync();
  if (wait() < 0) return -1;
  shutdown();
  return 0;
}

OperationInfos Process::getOperationInfos() {
  OperationInfos infos;
  nerikiri::foreach<Operation>(operations_, [&infos](auto& op) {
    infos.push_back(op.info());
  });
  return infos;
}


Operation& Process::getOperationByName(const std::string& name) {
  Operation& ret = Operation::null;
  nerikiri::foreach<Operation>(operations_, [&name, &ret](auto& op) {
    if (op.info().at("name").stringValue() == name) {
      ret = op;
    }
  });
  return ret;
}

Broker_ptr& Process::getBrokerByName(const std::string& name) {
  for(auto& brk : brokers_) {
    if (brk->info().at("name").stringValue() == name) {
      return brk;
    }
  }
  return Broker::null;
}

Broker_ptr& Process::getBrokerByBrokerInfo(const BrokerInfo& bi) {
  return getBrokerByName(bi.at("name").stringValue());
}

Value Process::invokeConnection(const Connection& con) {
  //auto& brokerProxy = 
  return Value();
}

Value Process::invokeOperationByName(const std::string& name) {
  logger::trace("Process::invokeOperationByName({})", name);
  auto& op = getOperationByName(name);
  if (op.isNull()) {
    return Value::error("Operation not found.");
  }
  /// ここで接続があったら、接続に対してデータをよこせという。なければデフォルト引数をそのまま与える
  Value ret(op.info().at("defaultArg").object_map<std::pair<std::string, Value>>([this, &op](const std::string& key, const Value& value) -> std::pair<std::string, Value>{
    for(auto& con : op.getConsumerConnectionsByArgName(key)) {
      if (con.isPull()) {
        return {key, invokeConnection(con)};
      }
    }
    return {key, value};
  }));

  return nerikiri::call_operation(op, std::move(ret));
}