#pragma once

#include "nerikiri/operationbase.h"


namespace nerikiri {

    class Topic : public OperationBase {

    public:
        Topic() : OperationBase() {} // NullTopic
        Topic(const Value& config) : OperationBase(config) {}
        virtual ~Topic() {}
    public:
        virtual Value call(const Value& value) override {
            //std::lock_guard<std::mutex> lock(mutex_);
            bool flag = false;
            argument_mutex_.lock();
            flag = argument_updated_;
            argument_mutex_.unlock();
            if (flag) {
                if (value.hasKey("data"))
                    outputBuffer_.push(value.at("data"));
                argument_mutex_.lock();
                argument_updated_ = false;
                argument_mutex_.unlock();
            } else {
               // std::cout << "memorized output" << std::endl;
            }
            return getOutput();
        }
    };


    class TopicFactory : public Object {
    protected:

        TopicFactory(bool dmy) : Object() {}
    public:
        TopicFactory() : TopicFactory({{"typeName", "TopicFactory"}}) {}
        TopicFactory(const Value& info) : Object(info) {}

        ~TopicFactory() {}

    public:
        std::shared_ptr<Topic> create(const Value& info) {
            auto i = nerikiri::merge(info, info_);
            return std::make_shared<Topic>(i); 
        }
    };

    class NullTopicFactory : public TopicFactory {
    private:

    public:
        NullTopicFactory() : TopicFactory(false) {}
        ~NullTopicFactory() {}

    public:
    public:
        std::shared_ptr<Topic> create(const Value& info) {
            return std::make_shared<Topic>(); // Null Topic
        }
    };

    inline std::shared_ptr<Topic> nullTopic() {
        return std::make_shared<Topic>();
    }
}