#include <nerikiri/topic.h>
#include "nerikiri/operation/operation_base.h"


using namespace nerikiri;


using TopicBase = nerikiri::OperationBase;

class Topic : public TopicBase {
public:
    Topic(const std::string& _fullName, const Value& defaultArgs = {}): OperationBase("Topic", "Topic", _fullName, defaultArgs) {}
    virtual ~Topic() {}
public:
    virtual Value call(const Value& value) override {
        return value.at("data");
    }
};

std::shared_ptr<TopicAPI> nerikiri::topic(const std::string& _fullName, const Value& defaultArgs) {
    return std::make_shared<Topic>(_fullName, defaultArgs);
}

class NullTopic : public TopicBase {
public:
    NullTopic(): OperationBase("NullTopic", "NullTopic", "null") {}
    virtual ~NullTopic() {}
public:
    virtual Value call(const Value& value) override {
        return Value::error(logger::error("NullTopic::call() called. Topic is null."));
    }
};

std::shared_ptr<TopicAPI> nerikiri::nullTopic() {
    return std::make_shared<NullTopic>();
}

class TopicFactory : public TopicFactoryAPI {
public:
    TopicFactory(const std::string& fullName) : TopicFactoryAPI("TopicFactory", fullName) {}
    virtual ~TopicFactory() {}

public:
    virtual std::string topicTypeFullName() const override { return "Topic"; }

public:
    virtual std::shared_ptr<TopicAPI> create(const std::string& fullName) override {
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
    virtual std::shared_ptr<TopicAPI> create(const std::string& fullName) override {
        return std::make_shared<NullTopic>(); // Null Topic
    }
};


std::shared_ptr<TopicFactoryAPI> nerikiri::topicFactory(const std::string& fullName) {
    return std::make_shared<TopicFactory>(fullName);
}

std::shared_ptr<TopicFactoryAPI> nerikiri::nullTopicFactory() {
    return std::make_shared<NullTopicFactory>();
}