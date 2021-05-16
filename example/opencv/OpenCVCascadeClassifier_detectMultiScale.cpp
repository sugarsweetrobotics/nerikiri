#include <juiz/juiz.h>
#include <juiz/container.h>

#include "OpenCVCascadeClassifier.h"

using namespace juiz;

extern "C" {

	JUIZ_OPERATION  void* OpenCVCascadeClassifier_detectMultiScale();

}


    JUIZ_OPERATION  void* OpenCVCascadeClassifier_detectMultiScale() {
        return containerOperationFactory<OpenCVCascadeClassifier>(
        {
          {"typeName", "detectMultiScale"},
          {"defaultArg", {
                {"image", {
                    {"rows", 0},
                    {"cols", 0},
                    {"channels", 3}
                }},
          }},
        },
        [](auto& container, auto arg) {
            //std::cout << "OpenCVCascadeClassifier_detectMultiScale" << std::endl;
            if (!container.classifier_) {
                return Value::error("OpenCVCascadeClassifier_detectMultiScale failed. Not initialized.");
            }

            if (arg.at("image").at("rows").intValue() == 0 || arg.at("image").at("cols").intValue() == 0) {
                return Value::error("Invalid Image Size. Row or Col is zero.");
            }

            cv::Mat frame0(arg.at("image").at("rows").intValue(), arg.at("image").at("cols").intValue(), CV_8UC3);
            memcpy(frame0.data, &arg.at("image").at("data").byteArrayValue()[0], frame0.rows * frame0.cols * 3);
            
            cv::Mat frame_gray;
            cv::cvtColor( frame0, frame_gray, cv::COLOR_BGR2GRAY );
            cv::equalizeHist( frame_gray, frame_gray );
            //-- Detect faces
            std::vector<cv::Rect> faces;
            container.classifier_->detectMultiScale( frame_gray, faces );

            auto vs = Value::list();;
            for(auto rect : faces) {
                //std::cout << " - rect(x:" << rect.x << ",y:" << rect.y << ",width:" << rect.width << ",height:" << rect.height << ")" << std::endl;
                vs.push_back(Value({
                    {"x", (int64_t)rect.x}, {"y", (int64_t)rect.y},
                    {"width", (int64_t)rect.width}, {"height", (int64_t)rect.height}
                }));
            }
            return vs;
        });
    }

