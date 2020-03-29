#pragma once

#include <thread>
#include <chrono>
#include "nerikiri/operation.h"

namespace nerikiri {
    class ExecutionContext {
    public:
        ExecutionContext() {}
        virtual ~ExecutionContext() {}
        std::vector<std::reference_wrapper<OperationBaseBase>> operations_;

    public:
        virtual bool start() = 0;
        virtual bool stop() = 0;


        virtual void svc() {
            for(auto op : operations_) {
                op.get().execute();
            }
        }
    };
    
    class TimerEC : public ExecutionContext {
    private:
        double rate_;
        bool flag_;
        std::thread* thread_;
    public:
        TimerEC(double rate) : ExecutionContext(), rate_(rate) {}
        virtual ~TimerEC() {}

    public:

        virtual bool start() {
            flag_ = false;
            thread_ = new std::thread([this]() {
                while(!flag_) {
                    svc();
                    std::this_thread::sleep_for(std::chrono::nanoseconds( (int)(1.0E+9/rate_) ));
                }
            });
            return true;
        }

        virtual bool stop() { 
            flag_ = true;
            thread_->join();
            return true;
        }
    };
}