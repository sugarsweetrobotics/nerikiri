#include "juiz/juiz.h"
#include "juiz/containeroperationfactory.h"

#include "OpenCVCascadeClassifier.h"

using namespace juiz;

extern "C" {

    JUIZ_OPERATION  void* OpenCVCascadeClassifier_initialize() {
        return containerOperationFactory<OpenCVCascadeClassifier>(
        {
          {"typeName", "initialize"},
          {"defaultArg", {
              {"parameterFilePath", {"/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_default.xml"}}}, // For OSX brew
          }},
        },
        [](auto& container, auto arg) {
            //std::cout << "OpenCVCascadeClassfier_initialize (arg:" << str(arg) << std::endl;
            container.classifier_ = std::make_shared<cv::CascadeClassfier>();
            container.classifier_->load(arg.at("parameterFilePath").stringValue());
            return arg;
        });
    }

}