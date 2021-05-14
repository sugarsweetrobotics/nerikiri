#pragma once

#include <nerikiri/object.h>
#include <nerikiri/operation_api.h>

namespace nerikiri {
    using TopicAPI = OperationAPI;
    std::shared_ptr<TopicAPI> nullTopic();
    
    class TopicFactoryAPI : public Object {
    public:
        TopicFactoryAPI(const std::string& typeName, const std::string& fullName) : Object(typeName, fullName) {}
        ~TopicFactoryAPI() {}
    public:
        virtual std::shared_ptr<TopicAPI> create(const std::string& fullName) = 0;
    };
    std::shared_ptr<TopicFactoryAPI> nullTopicFactory();

    template<>
    inline std::shared_ptr<TopicFactoryAPI> nullObject() { return nullTopicFactory(); }
}