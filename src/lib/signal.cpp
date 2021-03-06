#include <vector>
#include <csignal>
#include <condition_variable>

#include <juiz/utils/signal.h>



using namespace juiz;

namespace
{
  volatile std::sig_atomic_t signal_captured;
  std::condition_variable condition_var;
  std::mutex condition_mutex;
}
 
static void signal_handler(int signal)
{
  signal_captured = signal;
  condition_var.notify_all();
}
 

static int sig2sig(const juiz::Signal sig) {
  switch (sig) {
  case juiz::Signal::SIGNAL_INT:
    return SIGINT;
  default:
    return 0;
  }
}

bool juiz::wait_for(const Signal signal) {
  int sig = sig2sig(signal);
  if (sig) {
    std::unique_lock<std::mutex> lock(condition_mutex);
    std::signal(sig, signal_handler);
    condition_var.wait(lock);
    return true;
  }

  return false;
}
