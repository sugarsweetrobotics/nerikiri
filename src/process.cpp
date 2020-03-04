#include <sstream>
#include "nerikiri/nerikiri.h"
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
  operations_.emplace_back(std::move(op));
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
  brokerDictionary_.add(std::forward<Broker_ptr>(brk));
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
  auto broker_futures = brokerDictionary_.map<std::future<bool>>([this](auto& brk) {
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
  brokerDictionary_.foreach([](auto& brk) {
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
    