#pragma once


#include <nerikiri/topic_api.h>
#include <nerikiri/topic_factory_api.h>

namespace nerikiri {
    std::shared_ptr<TopicAPI> topic(const std::string& _fullName, const Value& defaultArgs = {});
 
    std::shared_ptr<TopicFactoryAPI> topicFactory(const std::string& fullName);
}