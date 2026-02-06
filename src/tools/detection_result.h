#pragma once

#include <vector>
#include <opencv2/core.hpp>

namespace tools {

enum class DetectionKind {
  None,
  Lines,
  Points,
  Circles,
  Mixed
};

struct DetectionResult {
  DetectionKind kind = DetectionKind::None;
  // Lines: Vec4i = (x1,y1,x2,y2)
  std::vector<cv::Vec4i> lines;
  // Points: 2D points
  std::vector<cv::Point2f> points;
  // Circles: Vec3f = (x,y,r)
  std::vector<cv::Vec3f> circles;
};

} // namespace tools
