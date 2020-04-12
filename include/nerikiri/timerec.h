#pragma once

#include "nerikiri/ec.h"

namespace nerikiri {


    class TimerEC : public ExecutionContext {
    private:
        double rate_;
        bool flag_;
        std::thread* thread_;
    public:
        TimerEC(const Value& info) : ExecutionContext(info) {
            if (info.at("rate").isDoubleValue()) {
                rate_ = (info.at("rate").doubleValue());
            } else if (info.at("rate").isIntValue()) {
                rate_ = info.at("rate").intValue();
            } else {
                logger::error("TimerEC creation failed. Rate must be double (or int) value.");
                rate_ = -1;
            }
        }
        virtual ~TimerEC() {}

    public:

        virtual bool onStarted() override {
            flag_ = false;
            thread_ = new std::thread([this]() {
                logger::trace("TimerEC::onStarted() in Thread starting....");
                while(!flag_) {
                    logger::trace("TimerEC::onStarted() in Thread svc calling");
                    svc();
                    std::this_thread::sleep_for(std::chrono::nanoseconds( (int)(1.0E+9/rate_) ));
                }
                logger::trace("TimerEC::onStarted() in Thread stopped");
            });
            return true;
        }

        virtual bool onStopping() override { 
            flag_ = true;
            thread_->join();
            return true;
        }
    };

    class TimerECFactory : public ExecutionContextFactory {
    public:
        TimerECFactory() {}
        virtual ~TimerECFactory() {}

    public:
        virtual std::shared_ptr<ExecutionContext> create(const Value& arg) const {
            return std::shared_ptr<ExecutionContext>(new TimerEC(arg));
        };

        virtual std::string typeName() const { 
            return "TimerEC";
        }
    };


}