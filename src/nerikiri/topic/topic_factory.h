#pragma once

#include <nerikiri/process_store.h>
#include <nerikiri/topic_api.h>

namespace nerikiri {


    std::shared_ptr<TopicAPI> createTopic(ProcessStore& store, const Value& info);

    Value publishTopic(ProcessStore& store, const std::shared_ptr<OperationAPI>& op, const Value& pubTopicInfo);

}