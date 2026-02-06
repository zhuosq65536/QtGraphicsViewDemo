#include "circle_tool.h"

using namespace tools;

DetectionResult CircleTool::run(const cv::Mat& image, const cv::Rect& roi) {
  DetectionResult res;
  res.kind = DetectionKind::Circles;

  if (image.empty()) return res;

  cv::Mat src = image;
  if (roi.width > 0 && roi.height > 0) {
    cv::Rect r = roi & cv::Rect(0,0, image.cols, image.rows);
    src = image(r);
  }

  cv::Mat gray;
  if (src.channels() == 3) cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
  else gray = src;
  cv::GaussianBlur(gray, gray, cv::Size(9,9), 2, 2);

  std::vector<cv::Vec3f> circles;
  cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, params.dp, params.minDist, params.param1, params.param2, params.minRadius, params.maxRadius);

  if (roi.width > 0 && roi.height > 0) {
    for (auto& c : circles) {
      c[0] += roi.x; c[1] += roi.y;
    }
  }

  res.circles = std::move(circles);
  return res;
}
