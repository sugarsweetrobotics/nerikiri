#include <sstream>

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
  return *this;
}

Process& Process::addBroker(Broker_ptr&& brk) {
  trace("Process::addBroker()");
  brokerDictionary_.add(std::forward<Broker_ptr>(brk));
  return *this;
}


void Process::startAsync() {
  logger::trace("Process::startAsync()");
  auto ret = brokerDictionary_.map<std::future<bool>>([this](auto& brk) {
							logger::trace("Creating a thread for broker {}", str(brk->info()));
							std::promise<bool> prms;
							auto ftr = prms.get_future();
							this->threads_.emplace_back(std::thread([&brk](std::promise<bool> p) {
												  std::stringstream ss;
												  ss << std::this_thread::get_id();
												  logger::trace("A thread {} is going to run broker {}", ss.str(), str(brk->info()));
												  p.set_value(brk->run());
												  logger::trace("A thread {} finished", ss.str());
												}, std::move(prms)));
							return ftr;
						      });
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

std::vector<OperationInfo> Process::getOperationInfos() {
  return {};
}
