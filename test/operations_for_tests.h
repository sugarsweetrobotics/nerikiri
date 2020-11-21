#pragma once
#include <memory>
#include "nerikiri/operation_factory.h"

bool operationIsCalled = false;


auto opf1 = std::make_shared<nerikiri::OperationFactory>(
    nerikiri::Value { 
        {"typeName", "inc"},
        {"defaultArg", {
            {"arg01", 1}
        }}
    },
    [](auto arg) {
        operationIsCalled = true;
        return nerikiri::Value(arg.at("arg01").intValue()+1);
    }
);

auto opf2 = std::make_shared<nerikiri::OperationFactory>(
    nerikiri::Value { 
        {"typeName", "zero"},
        {"defaultArg", { }}
    },
    [](auto arg)  {
        operationIsCalled = true;
        return nerikiri::Value(0);
    }
);