#pragma once

#include "detection_result.h"
#include <opencv2/core.hpp>

namespace tools {

class ITool {
public:
  virtual ~ITool() = default;
  // 输入图像及可选ROI，返回 DetectionResult
  virtual DetectionResult run(const cv::Mat& image, const cv::Rect& roi = cv::Rect()) = 0;
};

} // namespace tools
