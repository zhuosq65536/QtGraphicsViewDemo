#pragma once

#include "itool.h"
#include <opencv2/imgproc.hpp>

namespace tools {

class CircleTool : public ITool {
public:
  struct Params {
    double dp = 1.0;
    double minDist = 20.0;
    double param1 = 100.0; // canny high
    double param2 = 30.0;  // accumulator threshold
    int minRadius = 0;
    int maxRadius = 0;
  } params;

  DetectionResult run(const cv::Mat& image, const cv::Rect& roi = cv::Rect()) override;
};

} // namespace tools
