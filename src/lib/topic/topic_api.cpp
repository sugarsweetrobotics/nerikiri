#include <juiz/topic.h>
#include "../operation/operation_base.h"


using namespace juiz;


using TopicBase = juiz::OperationBase;

class Topic : public TopicBase {
public:
    Topic(const std::string& _fullName, const Value& defaultArgs = {{"data", {}}}): OperationBase("Topic", "Topic", _fullName, defaultArgs) {
        setDescription("Plain Topic");
    }
    virtual ~Topic() {}
public:
    virtual Value call(const Value& value) override {
        return value.at("data");
    }
};

std::shared_ptr<TopicAPI> juiz::topic(const std::string& _fullName, const Value& defaultArgs) {
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

std::shared_ptr<TopicAPI> juiz::nullTopic() {
    return std::make_shared<NullTopic>();
}

class TopicFactory : public TopicFactoryAPI {
public:
    TopicFactory() : TopicFactoryAPI("TopicFactory", "Topic") {}
    virtual ~TopicFactory() {}

public:
    virtual std::shared_ptr<TopicAPI> create(const std::string& fullName) override {
        logger::info("TopicFactory::create({}) called", fullName);
        return std::make_shared<Topic>(fullName); 
    }
};

class NullTopicFactory : public TopicFactoryAPI {
public:
    NullTopicFactory() : TopicFactoryAPI("NullTopicFactory", "null") {}
    virtual ~NullTopicFactory() {}
public:
    // virtual std::string topicTypeFullName() const override { return "NullTopic"; }

public:
    virtual std::shared_ptr<TopicAPI> create(const std::string& fullName) override {
        logger::error("NullTopicFactory::{}(fullName={}) called. Object is null", __func__, fullName);
        return nullTopic();
    }
};


std::shared_ptr<TopicFactoryAPI> juiz::topicFactory() {
    return std::make_shared<TopicFactory>();
}

std::shared_ptr<TopicFactoryAPI> juiz::nullTopicFactory() {
    return std::make_shared<NullTopicFactory>();
}