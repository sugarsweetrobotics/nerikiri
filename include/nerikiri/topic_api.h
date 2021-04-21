#pragma once

#include <nerikiri/object.h>
#include <nerikiri/operation_api.h>

namespace nerikiri {

    /*
    class TopicAPI : public OperationAPI {
    public:
        TopicAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : OperationAPI(className, typeName, fullName) {}
        virtual ~TopicAPI() {}
    };
    */

   using TopicAPI = OperationAPI;


    std::shared_ptr<TopicAPI> nullTopic();
   // template<> inline std::shared_ptr<TopicAPI> nullObject() { return nullTopic(); }
}