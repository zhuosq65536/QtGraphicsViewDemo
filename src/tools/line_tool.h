#pragma once

#include "itool.h"
#include <opencv2/imgproc.hpp>

namespace tools {

class LineTool : public ITool {
public:
  struct Params {
    double rho = 1.0;
    double theta = CV_PI/180.0;
    int threshold = 50;
    double minLineLength = 20.0;
    double maxLineGap = 10.0;
  } params;

  DetectionResult run(const cv::Mat& image, const cv::Rect& roi = cv::Rect()) override;
};

} // namespace tools
