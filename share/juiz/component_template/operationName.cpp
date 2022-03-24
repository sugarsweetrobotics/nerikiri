/**
 * @file operationName.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date $DATE
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <juiz/juiz.h>
#include <juiz/operation.h>

using namespace juiz;

extern "C" {
    /**
     * @brief Create operationFactory object for operationName operation
     * 
     * @return JUIZ_OPERATION* 
     */
    JUIZ_OPERATION void* operationName() {
        return operationFactory(
          {
            {"typeName", "operationName"},
            {"defaultArg", {
 //               {"int_arg_name", 0},
 //               {"double_arg_name", 0.0},
 //               {"string_arg_name", "string_value"}
            }},
          },
          [](auto arg) -> juiz::Value {
              return Value::error("operationName is not implemented yet.");
          });
    }
}
