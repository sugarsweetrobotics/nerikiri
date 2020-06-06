#include "nerikiri/nerikiri.h"
#include "nerikiri/containers/containeroperationfactory.h"

#include "OpenCVCamera.h"

using namespace nerikiri;

extern "C" {

    NK_OPERATION  void* OpenCVCamera_initialize() {
        return containerOperationFactory<OpenCVCamera>(
        {
          {"name", "initialize"},
          {"defaultArg", {
              {"cameraId", 0},
              {"width", 320},
              {"height", 240}
          }},
        },
        [](auto& container, auto arg) {
            //std::cout << "OpenCVCamera_initialize (arg:" << str(arg) << std::endl;
            container.capture_ = std::make_shared<cv::VideoCapture>(arg.at("cameraId").intValue());
            container.capture_->set(cv::CAP_PROP_FRAME_WIDTH, arg.at("width").intValue());
            container.capture_->set(cv::CAP_PROP_FRAME_HEIGHT, arg.at("height").intValue());
            //container.gamepad = std::make_shared<ssr::aqua2::GamePad>(arg.at("arg").stringValue().c_str());
            return arg;
        });
    }

}