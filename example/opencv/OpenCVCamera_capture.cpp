#include <nerikiri/nerikiri.h>
#include <nerikiri/container.h>

#include "OpenCVCamera.h"

using namespace nerikiri;

extern "C" {

	NK_OPERATION  void* OpenCVCamera_capture();
}



    NK_OPERATION  void* OpenCVCamera_capture() {
        return containerOperationFactory<OpenCVCamera>(
        {
          {"typeName", "capture"},
          {"defaultArg", {
              {"cameraId", "0"}
          }},
        },
        [](auto& container, auto arg) {
            if (container.capture_) {
            cv::Mat frame;
            if (!container.capture_->read(frame)) {
                return Value::error("VideoCapture failed.");
            }

            // std::cout << "rows:" << frame.rows << ", cols:" << frame.cols << ", channels:" << frame.channels() << ", size:" << frame.elemSize() << std::endl;

            return Value({
                {"__TYPE__", "__IMAGE__"},
                {"rows", (int64_t)frame.rows},
                {"cols", (int64_t)frame.cols},
                {"channels", frame.channels()},
                {"data", Value(frame.data, frame.rows * frame.cols * frame.elemSize())}
            });
            }

            return Value::error("OpenCVCamera_capture failed. Camera is not initialized.");
        });
    }

