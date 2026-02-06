#pragma once

#include "itool.h"
#include <opencv2/imgproc.hpp>

namespace tools {

class PointTool : public ITool {
public:
  struct Params {
    double max_corners = 500;
    double quality_level = 0.01;
    double min_distance = 10.0;
  } params;

  DetectionResult run(const cv::Mat& image, const cv::Rect& roi = cv::Rect()) override;
};

} // namespace tools
