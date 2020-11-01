#pragma once

#include <nerikiri/operation_base.h>


namespace nerikiri {

    using TopicBase = OperationBase;

    class Topic : public TopicBase {
    public:
        Topic(const std::string& _fullName, const Value& defaultArgs = {}): OperationBase("Topic", "Topic", _fullName, defaultArgs) {}
        virtual ~Topic() {}
    public:
        virtual Value call(const Value& value) override {
          return value.at("data");
        }
    };

    class NullTopic : public TopicBase {
    public:
        NullTopic(): OperationBase("NullTopic", "NullTopic", "null") {}
        virtual ~NullTopic() {}
    public:
        virtual Value call(const Value& value) override {
          return Value::error(logger::error("NullTopic::call() called. Topic is null."));
        }
    };


    class TopicFactoryAPI : public Object {
    public:
        TopicFactoryAPI(const std::string& typeName, const std::string& fullName) : Object(typeName, fullName) {}
        ~TopicFactoryAPI() {}

    public:
        virtual std::string topicTypeFullName() const = 0;
        virtual std::shared_ptr<TopicBase> create(const std::string& fullName) = 0;
    };

    class TopicFactory : public TopicFactoryAPI {
    public:
        TopicFactory(const std::string& fullName) : TopicFactoryAPI("TopicFactory", fullName) {}
        virtual ~TopicFactory() {}

    public:
        virtual std::string topicTypeFullName() const { return "Topic"; }

    public:
        virtual std::shared_ptr<TopicBase> create(const std::string& fullName) {
            logger::trace("TopicFactory::create({}) called", fullName);
            return std::make_shared<Topic>(fullName); 
        }
    };

    class NullTopicFactory : public TopicFactoryAPI {
    public:
        NullTopicFactory() : TopicFactoryAPI("NullTopicFactory", "null") {}
        virtual ~NullTopicFactory() {}
    public:
        virtual std::string topicTypeFullName() const override { return "NullTopic"; }

    public:
        virtual std::shared_ptr<TopicBase> create(const std::string& fullName) override {
            return std::make_shared<NullTopic>(); // Null Topic
        }
    };
}