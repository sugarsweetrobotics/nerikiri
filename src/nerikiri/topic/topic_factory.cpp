#include "topic_factory.h"

#include <nerikiri/objectfactory.h>
using namespace nerikiri;


Value topicConnectionInfo(const Value& value) {
    if (value.isStringValue()) {
        return {

        };
    }
    return value;
}


std::shared_ptr<TopicAPI> nerikiri::createTopic(ProcessStore& store, const Value& info) {
    ObjectFactory::createTopic(store, info);
    if (info.isStringValue()) {
        return store.get<TopicAPI>(Value::string(info));
    }
    return store.get<TopicAPI>(Value::string(info.at("name")));
}

Value nerikiri::publishTopic(ProcessStore& store, const std::shared_ptr<OperationAPI>& op, const Value& pubTopicInfo) {
    auto topic = createTopic(store, pubTopicInfo);
    auto info = topicConnectionInfo(pubTopicInfo);
    auto v = op->outlet()->connectTo(topic->inlet("data"), info);
    v = topic->inlet("data")->connectTo(op->outlet(), info);
    return v;
}