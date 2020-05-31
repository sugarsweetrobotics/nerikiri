#include "nerikiri/nerikiri.h"
#include "nerikiri/containers/containeroperationfactory.h"

#include "OpenCVCamera.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* OpenCVCamera_capture() {
        return containerOperationFactory<OpenCVCamera>(
        {
          {"name", "capture"},
          {"defaultArg", {
              {"cameraId", "0"}
          }},
        },
        [](auto& container, auto arg) {
            cv::Mat frame;
            if (!container.capture_->read(frame)) {
                return Value::error("VideoCapture failed.");
            }
            std::cout << "rows:" << frame.rows << ", cols:" << frame.cols << ", channels:" << frame.channels() << std::endl;
            
            return Value({});
        });
    }

}