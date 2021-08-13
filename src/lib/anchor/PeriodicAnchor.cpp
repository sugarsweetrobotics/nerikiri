#include <thread>

#include <juiz/ec.h>

using namespace juiz;

extern "C" {
    JUIZ_OPERATION  void* createPeriodicAnchor();
};


class PeriodicAnchor : public ExecutionContextBase {
private:
    double rate_;
    bool flag_;
    std::thread* thread_;
public:
    PeriodicAnchor(const Value& info) : ExecutionContextBase("PeriodicAnchor") {
        if (info.at("rate").isDoubleValue()) {
            rate_ = (info.at("rate").doubleValue());
        } else if (info.at("rate").isIntValue()) {
            rate_ = info.at("rate").intValue();
        } else {
            logger::error("PeriodicAnchor creation failed. Rate must be double (or int) value.");
            rate_ = -1;
        }
        //setDescription("タイマー実行コンテキスト．started状態ではbindされたOperationを周期的に実行します．");
    }

    virtual ~PeriodicAnchor() {}

public:

    virtual bool onStarted() override {
        flag_ = false;
        thread_ = new std::thread([this]() {
            logger::trace("PeriodicAnchor::onStarted() in Thread starting....");
            while(!flag_) {
                logger::trace("PeriodicAnchor::svc_thread. svc calling");
                svc();
                std::this_thread::sleep_for(std::chrono::nanoseconds( (int)(1.0E+9/rate_) ));
            }
            logger::trace("PeriodicAnchor::onStarted() in Thread stopped");
        });
        return true;
    }

    virtual bool onStopping() override { 
        flag_ = true;
        thread_->join();
        return true;
    }
};

std::shared_ptr<ExecutionContextFactoryAPI> periodicAnchor() {
   return std::make_shared<ECFactory<PeriodicAnchor>>();
}


void* createPeriodicAnchor() {
   return new ECFactory<PeriodicAnchor>();
}
