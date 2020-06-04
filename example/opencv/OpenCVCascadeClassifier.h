#pragma once



#include <memory>
#include <opencv2/opencv.hpp>


struct OpenCVCascadeClassifier {

    std::shared_ptr<cv::CascadeClassifier> classifier_;

};

