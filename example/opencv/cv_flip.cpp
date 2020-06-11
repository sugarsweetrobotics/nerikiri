#include "nerikiri/nerikiri.h"
#include "nerikiri/operationfactory.h"
#include <opencv2/opencv.hpp>

using namespace nerikiri;

extern "C" {
	NK_OPERATION  void* cv_flip();
}


    NK_OPERATION  void* cv_flip() {
        return operationFactory(
        {
          {"name", "cv_flip"},
          {"defaultArg", {
              {"image", {
                  {"rows", 0},
                  {"cols", 0},
                  {"channels", 3}
              }},
              {"flipCode", {1}}
          }},
        },
        [](auto arg) {
            //std::cout << "cv_flip called" << std::endl;
            if (arg.at("image").at("rows").intValue() == 0 || arg.at("image").at("cols").intValue() == 0) {
                return Value::error("Invalid Image Size. Row or Col is zero.");
            }
            cv::Mat frame0(arg.at("image").at("rows").intValue(), arg.at("image").at("cols").intValue(), CV_8UC3);
            //std::cout << "rows:" << frame0.rows << ", cols:" << frame0.cols << ", channels:" << frame0.channels() << ", size:" << frame0.elemSize() << std::endl;

            cv::Mat frame1(arg.at("image").at("rows").intValue(), arg.at("image").at("cols").intValue(), CV_8UC3);
            memcpy(frame0.data, &arg.at("image").at("data").byteArrayValue()[0], frame0.rows * frame0.cols * 3);
            cv::flip(frame0, frame1, arg.at("flipCode").intValue());
            
            return Value({
                {"__TYPE__", "__IMAGE__"},
                {"rows", (int64_t)frame1.rows},
                {"cols", (int64_t)frame1.cols},
                {"channels", frame1.channels()},
                {"data", Value(frame1.data, frame1.rows * frame1.cols * frame1.elemSize())}
            });
            
        });
    }

