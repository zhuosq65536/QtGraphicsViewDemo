#include "line_tool.h"

using namespace tools;

DetectionResult LineTool::run(const cv::Mat& image, const cv::Rect& roi) {
  DetectionResult res;
  res.kind = DetectionKind::Lines;

  if (image.empty()) return res;

  cv::Mat src = image;
  if (roi.width > 0 && roi.height > 0) {
    cv::Rect r = roi & cv::Rect(0,0, image.cols, image.rows);
    src = image(r);
  }

  cv::Mat gray, edges;
  if (src.channels() == 3) cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
  else gray = src;
  cv::GaussianBlur(gray, gray, cv::Size(3,3), 0);
  cv::Canny(gray, edges, 50, 150, 3);

  std::vector<cv::Vec4i> lines;
  cv::HoughLinesP(edges, lines, params.rho, params.theta, params.threshold, params.minLineLength, params.maxLineGap);

  // 如果有ROI需要修正坐标
  if (roi.width > 0 && roi.height > 0) {
    for (auto& l : lines) {
      l[0] += roi.x; l[1] += roi.y; l[2] += roi.x; l[3] += roi.y;
    }
  }

  res.lines = std::move(lines);
  return res;
}
