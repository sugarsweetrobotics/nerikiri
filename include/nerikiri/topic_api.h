#pragma once

#include <nerikiri/object.h>
#include <nerikiri/operation_api.h>

namespace nerikiri {

    using TopicAPI = OperationAPI;


    std::shared_ptr<TopicAPI> nullTopic();
}