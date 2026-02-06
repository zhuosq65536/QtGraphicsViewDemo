#include "point_tool.h"

using namespace tools;

DetectionResult PointTool::run(const cv::Mat& image, const cv::Rect& roi) {
  DetectionResult res;
  res.kind = DetectionKind::Points;

  if (image.empty()) return res;

  cv::Mat src = image;
  if (roi.width > 0 && roi.height > 0) {
    cv::Rect r = roi & cv::Rect(0,0, image.cols, image.rows);
    src = image(r);
  }

  cv::Mat gray;
  if (src.channels() == 3) cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
  else gray = src;

  std::vector<cv::Point2f> corners;
  cv::goodFeaturesToTrack(gray, corners, params.max_corners, params.quality_level, params.min_distance);

  // 如果有ROI需要修正坐标
  if (roi.width > 0 && roi.height > 0) {
    for (auto& p : corners) {
      p.x += roi.x; p.y += roi.y;
    }
  }

  res.points = std::move(corners);
  return res;
}
