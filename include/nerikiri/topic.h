#pragma once

#include <nerikiri/operation_api.h>


namespace nerikiri {

    using TopicAPI = OperationAPI;

    std::shared_ptr<TopicAPI> topic(const std::string& _fullName, const Value& defaultArgs = {});
    std::shared_ptr<TopicAPI> nullTopic();
 
    class TopicFactoryAPI : public Object {
    public:
        TopicFactoryAPI(const std::string& typeName, const std::string& fullName) : Object(typeName, fullName) {}
        ~TopicFactoryAPI() {}
    public:
        virtual std::string topicTypeFullName() const = 0;
        virtual std::shared_ptr<TopicAPI> create(const std::string& fullName) = 0;
    };

    std::shared_ptr<TopicFactoryAPI> topicFactory(const std::string& fullName);

    std::shared_ptr<TopicFactoryAPI> nullTopicFactory();

}