#pragma once

#include <nerikiri/object.h>

namespace nerikiri {




    class ProcessAPI : public Object {

    private:
    
    public:
        ProcessAPI(const std::string& className, const std::string& typeName, const std::string& fullName) : Object(className, typeName, fullName) {}
        virtual ~ProcessAPI() {}


    public:
        virtual Value fullInfo() const = 0;

    };

}