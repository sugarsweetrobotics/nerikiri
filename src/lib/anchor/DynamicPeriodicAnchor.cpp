#include <thread>
#include <iostream>
#include <juiz/ec.h>

using namespace juiz;

extern "C" {
    JUIZ_OPERATION  void* createDynamicPeriodicAnchor();
};


class DynamicPeriodicAnchor : public ExecutionContextBase {
private:
    double rate_;
    bool flag_;
    std::thread* thread_;
public:
    DynamicPeriodicAnchor(const Value& info) : ExecutionContextBase("DynamicPeriodicAnchor") {
        if (info.at("rate").isDoubleValue()) {
            rate_ = (info.at("rate").doubleValue());
        } else if (info.at("rate").isIntValue()) {
            rate_ = info.at("rate").intValue();
        } else {
            logger::error("DynamicPeriodicAnchor creation failed. Rate must be double (or int) value.");
            rate_ = -1;
        }
        //setDescription("タイマー実行コンテキスト．started状態ではbindされたOperationを周期的に実行します．");
    }

    virtual ~DynamicPeriodicAnchor() {}

public:

    virtual bool onStarted() override {
        flag_ = false;
        thread_ = new std::thread([this]() {
            logger::trace("DynamicPeriodicAnchor::onStarted() in Thread starting....");
            while(!flag_) {
                logger::trace("DynamicPeriodicAnchor::svc_thread. svc calling");
                svc();
                std::this_thread::sleep_for(std::chrono::nanoseconds( (int)(1.0E+9/rate_) ));
            }
            logger::trace("DynamicPeriodicAnchor::onStarted() in Thread stopped");
        });
        return true;
    }

    virtual bool onStopping() override { 
        flag_ = true;
        thread_->join();
        return true;
    }
};

std::shared_ptr<ExecutionContextFactoryAPI> dynamicPeriodicAnchor() {
   return std::make_shared<ECFactory<DynamicPeriodicAnchor>>();
}


void* createDynamicPeriodicAnchor() {
    logger::trace("DynamicPeriodicAnchor.cpp: createDynamicPeriodicAnchor() called");
   return new ECFactory<DynamicPeriodicAnchor>();
}
