#include "topic_factory.h"

#include "../objectfactory.h"
using namespace juiz;


Value topicConnectionInfo(const Value& value) {
    if (value.isStringValue()) {
        return {

        };
    }
    return value;
}


std::shared_ptr<TopicAPI> juiz::createTopic(ProcessStore& store, const Value& info) {
    ObjectFactory::createTopic(store, info);
    if (info.isStringValue()) {
        return store.get<TopicAPI>(Value::string(info));
    }
    return store.get<TopicAPI>(Value::string(info.at("name")));
}

Value juiz::publishTopic(ProcessStore& store, const std::shared_ptr<OperationAPI>& op, const Value& pubTopicInfo) {
    auto topic = createTopic(store, pubTopicInfo);
    auto info = topicConnectionInfo(pubTopicInfo);
    auto v = op->outlet()->connectTo(topic->inlet("data"), info);
    v = topic->inlet("data")->connectTo(op->outlet(), info);
    return v;
}