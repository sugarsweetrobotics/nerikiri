#include <iostream>

#include <juiz/nerikiri.h>
#include <juiz/operation.h>

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* joy2turtlevel() {
        return operationFactory(
            {
                {"typeName", "joy2turtlevel"},
                {"defaultArg", {
                    {"data", {
                        {"axis", Value(std::vector<double>{0.,0.,0.,0.,0.,0.})}
                    }}
                }}
            },
            [](auto arg_data) {
                //std::cout << "joy2turtlevel" << std::endl;
                auto arg = arg_data.at("data");
                double vx = -arg.at("axis").listValue()[1].doubleValue();
                double vth = -arg.at("axis").listValue()[0].doubleValue();
                return Value{
                    {"name", "turtle0"},
                    {"vx", vx},
                    {"vy", 0.0},
                    {"vth", vth}
                };
            });
    }

}