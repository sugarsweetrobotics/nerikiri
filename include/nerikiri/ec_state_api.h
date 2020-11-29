#pragma once

#include <nerikiri/object.h>

namespace nerikiri {




    class ECStateAPI : public Object {
    public:
        ECStateAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}

        virtual ~ECStateAPI() {}

    public:
        virtual Value activate() = 0;

    };


}