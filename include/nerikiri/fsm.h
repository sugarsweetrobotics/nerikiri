#pragma once

#include <string>
#include <map>

#include "nerikiri/nerikir.h"
#include "nerikiri/object.h"



namespace nerikiri {

    class NK_API FSM {
    private:
        
    public:
        FSM(const Value& info);

        virtual ~FSM();

    private:
        bool _isTransitable(const std::string& current, const std::string& next);

    public:
        std::string getState() const;
        
        Value setState(const std::string& state);

    };
}