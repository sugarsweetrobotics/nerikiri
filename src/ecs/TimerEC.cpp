#include <thread>

#include <juiz/ec.h>

using namespace juiz;

extern "C" {
    JUIZ_OPERATION  void* createTimerEC();
};


class TimerEC : public ExecutionContextBase {
private:
    double rate_;
    bool flag_;
    std::thread* thread_;
public:
    TimerEC(const Value& info) : ExecutionContextBase("TimerEC") {
        if (info.at("rate").isDoubleValue()) {
            rate_ = (info.at("rate").doubleValue());
        } else if (info.at("rate").isIntValue()) {
            rate_ = info.at("rate").intValue();
        } else {
            logger::error("TimerEC creation failed. Rate must be double (or int) value.");
            rate_ = -1;
        }
        //setDescription("タイマー実行コンテキスト．started状態ではbindされたOperationを周期的に実行します．");
    }

    virtual ~TimerEC() {}

public:

    virtual bool onStarted() override {
        flag_ = false;
        thread_ = new std::thread([this]() {
            logger::trace("TimerEC::onStarted() in Thread starting....");
            while(!flag_) {
                logger::verbose("TimerEC::svc_thread(rate={}). svc calling", rate_);
                svc();
                logger::verbose("TimerEC::svc_thread(rate={}). svc called", rate_);
                std::this_thread::sleep_for(std::chrono::nanoseconds( (int)(1.0E+9/rate_) ));
                logger::verbose("TimerEC::svc_thread(rate={}). sleeped", rate_);
            }
            logger::trace("TimerEC::onStarted() in Thread stopped");
        });
        return true;
    }

    virtual bool onStopping() override { 
        flag_ = true;
        thread_->join();
        thread_ = nullptr;
        return true;
    }
};

void* createTimerEC() {
   return new ECFactory<TimerEC>();
}
