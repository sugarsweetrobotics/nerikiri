#pragma once


#include <juiz/topic_api.h>

namespace juiz {
    std::shared_ptr<TopicAPI> topic(const std::string& _fullName, const Value& defaultArgs = {});
 
    std::shared_ptr<TopicFactoryAPI> topicFactory();
}