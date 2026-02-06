#pragma once

#include <QMainWindow>
#include <QWheelEvent>
#include <QPointF>
#include <QMouseEvent>
#include <opencv2/core.hpp>      // cv::Vec4i
#include <vector>     
// 前向声明
class QSplitter;
class QWidget;
class QVBoxLayout;
class QPushButton;
class QLineEdit;
class QLabel;
class QFrame;
class QDoubleSpinBox;
class QSpinBox;

class QGraphicsScene;
class QGraphicsPixmapItem;
class CustomGraphicsView;
class QStackedWidget;

// 工具接口与结果
namespace tools {
  class ITool;
  struct DetectionResult;
}

// 新增：OpenCV 前向声明（避免直接包含头文件）
namespace cv {
  class Mat;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;

protected:
  void wheelEvent(QWheelEvent* event) override;

private slots:
  void open_image_file();
  // 新增：找线工具相关槽函数
  void on_find_line_tool_clicked();    // 点击找线工具显示参数面板
  void on_execute_tool_clicked(); // 执行当前工具逻辑
  void on_point_tool_clicked(); // 找点工具
  void on_circle_tool_clicked(); // 找圆工具

private:
  QGraphicsScene* scene_ = nullptr;
  CustomGraphicsView* view_ = nullptr;
  QGraphicsPixmapItem* pixmap_item_ = nullptr;

  // 新增：找线工具参数控件（方便后续访问参数值）
  QDoubleSpinBox* rho_spin_ = nullptr;       // 霍夫检测rho参数
  QDoubleSpinBox* theta_spin_ = nullptr;     // theta参数
  QSpinBox* threshold_spin_ = nullptr;       // 阈值
  QDoubleSpinBox* min_line_len_spin_ = nullptr; // 最小线长
  QDoubleSpinBox* max_line_gap_spin_ = nullptr; // 最大线间隙
  // Point tool params
  QSpinBox* point_max_corners_spin_ = nullptr;
  QDoubleSpinBox* point_quality_spin_ = nullptr;
  QDoubleSpinBox* point_min_distance_spin_ = nullptr;
  // Circle tool params
  QDoubleSpinBox* circle_dp_spin_ = nullptr;
  QDoubleSpinBox* circle_min_dist_spin_ = nullptr;
  QDoubleSpinBox* circle_param1_spin_ = nullptr;
  QDoubleSpinBox* circle_param2_spin_ = nullptr;
  QSpinBox* circle_min_radius_spin_ = nullptr;
  QSpinBox* circle_max_radius_spin_ = nullptr;
  QWidget* param_panel_ = nullptr;           // 参数面板容器（控制显示/隐藏）
  // 新增：选项卡与页面引用，便于在槽函数中切换页面
  class QTabWidget* tabs_ = nullptr;
  QWidget* element_tab_ = nullptr;
  // 元素页内部使用栈来切换：默认是工具列表页，点击工具切换到参数页
  QStackedWidget* element_stack_ = nullptr;
  QWidget* element_list_page_ = nullptr;
  // 当前选中的工具
  enum class ToolType { None, Line, Point, Circle };
  ToolType current_tool_ = ToolType::None;
  void show_param_for_tool(ToolType t);
  // parameter widget groups
  QWidget* line_param_widget_ = nullptr;
  QWidget* point_param_widget_ = nullptr;
  QWidget* circle_param_widget_ = nullptr;
  QPushButton* execute_btn_ = nullptr;

  void init_ui();
  QWidget* create_tool_panel();
  // 新增：OpenCV 找线核心函数
  void find_lines_with_opencv();
  // 新增：QPixmap 转 cv::Mat
  cv::Mat qpixmap_to_cvmat(const QPixmap& pixmap);
  // 新增：绘制检测到的直线到场景
  void draw_lines_to_scene(const std::vector<cv::Vec4i>& lines);
  void draw_points_to_scene(const std::vector<cv::Point2f>& points);
  void draw_circles_to_scene(const std::vector<cv::Vec3f>& circles);
};