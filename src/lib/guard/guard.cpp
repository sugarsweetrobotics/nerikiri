#include <juiz/juiz.h>
#include <juiz/operation.h>
#include <opencv2/opencv.hpp>

using namespace juiz;

extern "C" {
	JUIZ_OPERATION  void* guard();
}


bool 
juiz::setupGuard(juiz::ProcessStore& store) {

    return true;
}

JUIZ_OPERATION  void* guard() {

    return operationFactory({
            {"typeName", "guard"},
            {"defaultArg", {
                {"_data_key", "data"},
                {"_operator", "=="},
                {"_literal", 0}
            }},
        },
        [](auto arg) {
            if (arg["_operator"].stringValue() == "==") {
                Value& literal = arg["_literal"];
                if (literal.isIntType()) {

                }
                auto v = arg[arg["_data_key"].stringValue()];
                if (v.type)
            }
            return Value({});
            
        });
}

