#pragma once

#include <nerikiri/object.h>

namespace nerikiri {




    class ProcessAPI : public Object {

    private:
    
    public:
        ProcessAPI(const std::string& typeName, const std::string& fullName) : Object(typeName, fullName) {}
        virtual ~ProcessAPI() {}

    };

}