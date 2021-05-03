#include <nerikiri/nerikiri.h>
#include <nerikiri/operation.h>
#include <opencv2/opencv.hpp>

using namespace nerikiri;

extern "C" {

	NK_OPERATION void* cv_drawRects();

}


    NK_OPERATION void* cv_drawRects() {
        return operationFactory(
        {
          {"typeName", "cv_drawRects"},
          {"defaultArg", {
                {"image", {
                    {"rows", 0},
                    {"cols", 0},
                    {"channels", 3}
                }},
                {"rects", Value::list()},
          }},
        },
        [](auto arg) {
            //std::cout << "cv_drawRects" << std::endl;
            if (arg.at("image").at("rows").intValue() == 0 || arg.at("image").at("cols").intValue() == 0) {
                return Value::error("Invalid Image Size. Row or Col is zero.");
            }

            cv::Mat frame(arg.at("image").at("rows").intValue(), arg.at("image").at("cols").intValue(), CV_8UC3);
            memcpy(frame.data, &arg.at("image").at("data").byteArrayValue()[0], frame.rows * frame.cols * 3);

            arg.at("rects").const_list_for_each([&frame](auto& v) {
                cv::Rect face(v.at("x").intValue(), v.at("y").intValue(), v.at("width").intValue(), v.at("height").intValue());
                cv::Point center( face.x + face.width/2, face.y + face.height/2 );
                cv::ellipse( frame, center, cv::Size( face.width/2, face.height/2 ), 0, 0, 360, cv::Scalar( 255, 0, 255 ), 4 );
            });

            return Value({
                {"__TYPE__", "__IMAGE__"},
                {"rows", (int64_t)frame.rows},
                {"cols", (int64_t)frame.cols},
                {"channels", frame.channels()},
                {"data", Value(frame.data, frame.rows * frame.cols * frame.elemSize())}
            });
        });
    }

