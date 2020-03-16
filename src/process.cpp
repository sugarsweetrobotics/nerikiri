#include <sstream>
#include "nerikiri/nerikiri.h"
#include "nerikiri/functional.h"
#include "nerikiri/process.h"
#include "nerikiri/logger.h"
#include "nerikiri/signal.h"


using namespace nerikiri;
using namespace nerikiri::logger;


Process Process::null("");

Process::Process(const std::string& name) : info_({{"name", name}}) {
  logger::trace("Process::Process(\"{}\")", name);
  if (name.length() == 0) { 
    info_ = Value::error("Process is Null."); 
  } 
  setLogLevel(TRACE);
}

Process::~Process() {
  logger::trace("Process::~Process()");
}

Process& Process::addOperation(Operation&& op) {
  logger::trace("Process::addOperation({})", str(op.info()));
  if (!getOperationByName(info().at("name").stringValue()).isNull()) {
    logger::error("Process::addOperation({}) Error. Process already has the same name operation", info().at("name").stringValue());
    return *this;
  }
  operations_.emplace_back(std::move(op));
  return *this;
}

Process& Process::addOperation(const Operation& op) {
  logger::trace("Process::addOperation({})", str(op.info()));
  if (!getOperationByName(info().at("name").stringValue()).isNull()) {
    logger::error("Process::addOperation({}) Error. Process already has the same name operation", info().at("name").stringValue());
    return *this;
  }
  operations_.push_back(op);
  return *this;
}

Process& Process::addSystemEditor(SystemEditor_ptr&& se) {
  logger::trace("Process::addSystemEditor()");
  systemEditors_.emplace(se->name(), std::forward<SystemEditor_ptr>(se));
  return *this;
}

Process& Process::addBroker(Broker_ptr&& brk) {
  logger::trace("Process::addBroker()");
  brk->setProcess(Process_ptr(this));
  brokers_.emplace_back(std::forward<Broker_ptr>(brk));
  return *this;
}

static auto start_broker(std::vector<std::thread>& threads, Broker_ptr& brk) {
  logger::trace("Creating a thread for broker {}", str(brk->getBrokerInfo()));
  std::promise<bool> prms;
  auto ftr = prms.get_future();
  threads.emplace_back(std::thread([&brk](std::promise<bool> p) {
				     std::stringstream ss;
				     ss << std::this_thread::get_id();
				     logger::trace("A thread {} is going to run broker {}", ss.str(), str(brk->getBrokerInfo()));
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

Value Process::getOperationInfos() {
  return {nerikiri::map<Value, Operation>(operations_, [](auto& op) { return op.info();})};
}

Value Process::getContainerInfos() {
  return {nerikiri::map<Value, ContainerBase*>(containers_, [](auto* ctn) { return ctn->info(); })};
}

Operation& Process::getOperationByName(const std::string& name) {
  Operation& null = Operation::null;
  for(auto& op : operations_) {
    if (op.info().at("name").stringValue() == name) return op;
  }
  return Operation::null;
}


Operation& Process::getOperationByInfo(const OperationInfo& oi) {
  return getOperationByName(oi.at("name").stringValue());
}

Broker_ptr Process::getBrokerByName(const std::string& name) {
  for(auto& brk : brokers_) {
    if (brk->getBrokerInfo().at("name").stringValue() == name) {
      return brk;
    }
  }
  return Broker::null;
}

Broker_ptr Process::getBrokerByInfo(const BrokerInfo& bi) {
  return getBrokerByName(bi.at("name").stringValue());
}

Process& Process::addContainer(ContainerBase* container) {
  trace("Process::addContainer()");
  containers_.emplace_back(container);
  return *this;
}

Process& Process::addOperationToContainerByName(const std::string&name, ContainerOperationBase* operation) {
  getContainerByName(name).addOperation(operation);
  return *this;
}

ContainerBase& Process::getContainerByName(const std::string& name) {
  auto cs = nerikiri::filter<ContainerBase*>(containers_, [&name](auto c){return c->info().at("name").stringValue() == name;});
  if (cs.size() == 1) return *cs[0];
  if (cs.size() == 0) {
    logger::warn("Process::getContainerByName({}) failed. Not found.", name);
    return ContainerBase::null;
  } else {
    logger::warn("Process::getContainerByName({}) failed. Multiple containers with the same name are found.", name);
    return ContainerBase::null;

  }
}