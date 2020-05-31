#pragma once

#include <memory>
#include <opencv2/opencv.hpp>


struct OpenCVCamera {

    std::shared_ptr<cv::VideoCapture> capture_;
};

