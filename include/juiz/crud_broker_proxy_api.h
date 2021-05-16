#pragma once

#include <juiz/value.h>

namespace juiz {


    class CRUDBrokerProxyAPI {
    public:
        virtual ~CRUDBrokerProxyAPI () {}

    public:

        virtual Value createResource(const std::string& fullpath, const Value& info) = 0;

        virtual Value readResource(const std::string& fullName) const = 0;

        virtual Value updateResource(const std::string& fullName, const Value& info) = 0;

        virtual Value deleteResource(const std::string& fullName) = 0;

    };

}