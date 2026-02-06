#pragma once

#include "mainwindow.h"
#include "custom_graphics_view.h"
// Qt 头文件
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFileDialog>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QMenuBar>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QTextCodec>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGraphicsLineItem>
#include <QTabWidget>
#include <QStackedWidget>
// tools
#include "tools/line_tool.h"
#include "tools/point_tool.h"
#include "tools/circle_tool.h"
// 新增：OpenCV 头文件
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  init_ui();
}

void MainWindow::draw_points_to_scene(const std::vector<cv::Point2f>& points) {
  if (points.empty()) {
    QMessageBox::information(this, tr(u8"提示"), tr(u8"未检测到点！"));
    return;
  }
  QPen ppen(Qt::red);
  for (const auto& pt : points) {
    QGraphicsEllipseItem* it = new QGraphicsEllipseItem(pt.x-3, pt.y-3, 6, 6);
    it->setPen(ppen);
    it->setBrush(QBrush(Qt::red));
    scene_->addItem(it);
  }
  QMessageBox::information(this, tr(u8"完成"), tr(u8"共检测到 %1 个点！").arg(points.size()));
}

void MainWindow::draw_circles_to_scene(const std::vector<cv::Vec3f>& circles) {
  if (circles.empty()) {
    QMessageBox::information(this, tr(u8"提示"), tr(u8"未检测到圆！"));
    return;
  }
  QPen cpen(Qt::yellow);
  for (const auto& c : circles) {
    float x = c[0], y = c[1], r = c[2];
    QGraphicsEllipseItem* it = new QGraphicsEllipseItem(x-r, y-r, r*2, r*2);
    it->setPen(cpen);
    it->setBrush(Qt::NoBrush);
    scene_->addItem(it);
  }
  QMessageBox::information(this, tr(u8"完成"), tr(u8"共检测到 %1 个圆！").arg(circles.size()));
}

MainWindow::~MainWindow() = default;

void MainWindow::init_ui() {
  // 1. 菜单栏
  QMenu* file_menu = menuBar()->addMenu(tr(u8"文件"));
  QAction* open_action = file_menu->addAction(tr(u8"打开图片"));
  connect(open_action, &QAction::triggered, this, &MainWindow::open_image_file);

  // 2. 创建场景
  scene_ = new QGraphicsScene(this);
  scene_->setSceneRect(0, 0, 800, 600);

  // 3. 创建自定义视图
  view_ = new CustomGraphicsView(this);
  view_->setScene(scene_);

  // 左右分栏布局
  QSplitter* main_splitter = new QSplitter(Qt::Horizontal, this);
  main_splitter->setHandleWidth(2);
  main_splitter->setStyleSheet("QSplitter::handle { background-color: #E0E0E0; }");

  main_splitter->addWidget(view_);
  main_splitter->setStretchFactor(0, 8);

  QWidget* tool_panel = create_tool_panel();
  main_splitter->addWidget(tool_panel);
  main_splitter->setStretchFactor(1, 2);

  setCentralWidget(main_splitter);

  resize(1200, 800);
  setWindowTitle(tr(u8"Qt Graphics View - 图片显示与矩形绘制"));
}

QWidget* MainWindow::create_tool_panel() {
  QWidget* panel = new QWidget(this);
  panel->setMinimumWidth(360); // 加宽面板，适配参数输入，避免切换标签时出现滚动条
  panel->setStyleSheet("background-color: #F5F5F5; padding: 10px;");
  QVBoxLayout* layout = new QVBoxLayout(panel);
  layout->setSpacing(10);
  layout->setContentsMargins(10, 10, 10, 10);

  // 面板标题
  QLabel* title_label = new QLabel(tr(u8"工具面板"), panel);
  title_label->setStyleSheet("font-size: 14px; font-weight: bold; color: #333333;");
  layout->addWidget(title_label);

  QFrame* line = new QFrame(panel);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  line->setStyleSheet("background-color: #CCCCCC;");
  layout->addWidget(line);

  // 使用选项卡来切换不同工具页
  QTabWidget* tabs = new QTabWidget(panel);
  tabs->setContentsMargins(0, 0, 0, 0);
  // 保存 tabs 到成员，便于在槽函数中切换
  tabs_ = tabs;

  // 高级工具页（占位）
  QWidget* advanced_tab = new QWidget(tabs);
  QVBoxLayout* adv_layout = new QVBoxLayout(advanced_tab);
  adv_layout->setContentsMargins(8, 8, 8, 8);
  adv_layout->addWidget(new QLabel(tr(u8"高级工具（占位）"), advanced_tab));
  adv_layout->addStretch();

  // 基础工具页（占位）
  QWidget* basic_tab = new QWidget(tabs);
  QVBoxLayout* basic_layout = new QVBoxLayout(basic_tab);
  basic_layout->setContentsMargins(8, 8, 8, 8);
  basic_layout->addWidget(new QLabel(tr(u8"基础工具（占位）"), basic_tab));
  basic_layout->addStretch();

  // 元素工具页（放置找线等元素级工具）
  QWidget* element_tab = new QWidget(tabs);
  element_tab_ = element_tab;
  QVBoxLayout* elem_layout = new QVBoxLayout(element_tab);
  elem_layout->setSpacing(10);
  elem_layout->setContentsMargins(8, 8, 8, 8);

  // 元素页内部使用栈式页面：工具列表页 和 参数页
  QStackedWidget* element_stack = new QStackedWidget(element_tab);
  element_stack_ = element_stack;

  // 工具列表页（包含找线按钮等）
  QWidget* element_list_page = new QWidget(element_stack);
  element_list_page_ = element_list_page;
  QVBoxLayout* list_layout = new QVBoxLayout(element_list_page);
  list_layout->setSpacing(10);
  list_layout->setContentsMargins(0, 0, 0, 0);

  // 找线工具按钮
  QPushButton* find_line_btn = new QPushButton(tr(u8"找线工具"), element_list_page);
  find_line_btn->setStyleSheet(R"(
    QPushButton {
      background-color: #FFFFFF;
      border: 1px solid #CCCCCC;
      border-radius: 4px;
      padding: 8px;
      font-size: 12px;
    }
    QPushButton:hover {
      background-color: #E6F7FF;
      border-color: #1890FF;
    }
    QPushButton:pressed {
      background-color: #BAE7FF;
    }
  )");
  find_line_btn->setMinimumHeight(40);
  connect(find_line_btn, &QPushButton::clicked, this, &MainWindow::on_find_line_tool_clicked);
  list_layout->addWidget(find_line_btn);

  // 找点工具按钮
  QPushButton* find_point_btn = new QPushButton(tr(u8"找点工具"), element_list_page);
  find_point_btn->setMinimumHeight(40);
  connect(find_point_btn, &QPushButton::clicked, this, &MainWindow::on_point_tool_clicked);
  list_layout->addWidget(find_point_btn);

  // 找圆工具按钮
  QPushButton* find_circle_btn = new QPushButton(tr(u8"找圆工具"), element_list_page);
  find_circle_btn->setMinimumHeight(40);
  connect(find_circle_btn, &QPushButton::clicked, this, &MainWindow::on_circle_tool_clicked);
  list_layout->addWidget(find_circle_btn);
  list_layout->addStretch();

  // ========== 新增：找线参数配置页（栈内页面） ==========
  QWidget* param_panel = new QWidget(element_stack);
  param_panel_ = param_panel; // 兼容旧变量名
  param_panel->setStyleSheet("background-color: #FFFFFF; padding: 8px; border-radius: 4px; border: 1px solid #EEEEEE;");
  QVBoxLayout* param_layout = new QVBoxLayout(param_panel);
  param_layout->setSpacing(8);
  param_layout->setContentsMargins(8, 8, 8, 8);

  // 将线性工具参数放到单独的容器，便于切换显示
  QWidget* line_param_widget = new QWidget(param_panel);
  line_param_widget_ = line_param_widget;
  QVBoxLayout* line_param_layout = new QVBoxLayout(line_param_widget);
  line_param_layout->setContentsMargins(0,0,0,0);

  // 1. rho 参数（霍夫检测的距离分辨率，单位：像素）
  QHBoxLayout* rho_layout = new QHBoxLayout();
  rho_layout->addWidget(new QLabel(tr(u8"Rho (像素):")));
  rho_spin_ = new QDoubleSpinBox();
  rho_spin_->setRange(0.1, 10.0);
  rho_spin_->setSingleStep(0.1);
  rho_spin_->setValue(1.0); // 默认值
  rho_layout->addWidget(rho_spin_);
  line_param_layout->addLayout(rho_layout);

  // 2. theta 参数（角度分辨率，单位：弧度）
  QHBoxLayout* theta_layout = new QHBoxLayout();
  theta_layout->addWidget(new QLabel(tr(u8"Theta (弧度):")));
  theta_spin_ = new QDoubleSpinBox();
  theta_spin_->setRange(0.001, CV_PI / 2);
  theta_spin_->setSingleStep(0.01);
  theta_spin_->setValue(CV_PI / 180); // 默认1度（弧度）
  line_param_layout->addLayout(theta_layout);

  // 3. 阈值参数
  QHBoxLayout* threshold_layout = new QHBoxLayout();
  threshold_layout->addWidget(new QLabel(tr(u8"阈值:")));
  threshold_spin_ = new QSpinBox();
  threshold_spin_->setRange(1, 200);
  threshold_spin_->setSingleStep(1);
  threshold_spin_->setValue(50); // 默认值
  threshold_layout->addWidget(threshold_spin_);
  line_param_layout->addLayout(threshold_layout);

  // 4. 最小线长
  QHBoxLayout* min_len_layout = new QHBoxLayout();
  min_len_layout->addWidget(new QLabel(tr(u8"最小线长 (像素):")));
  min_line_len_spin_ = new QDoubleSpinBox();
  min_line_len_spin_->setRange(1.0, 1000.0);
  min_line_len_spin_->setSingleStep(1.0);
  min_line_len_spin_->setValue(20.0); // 默认值
  min_len_layout->addWidget(min_line_len_spin_);
  line_param_layout->addLayout(min_len_layout);

  // 5. 最大线间隙
  QHBoxLayout* max_gap_layout = new QHBoxLayout();
  max_gap_layout->addWidget(new QLabel(tr(u8"最大线间隙 (像素):")));
  max_line_gap_spin_ = new QDoubleSpinBox();
  max_line_gap_spin_->setRange(0.0, 100.0);
  max_line_gap_spin_->setSingleStep(1.0);
  max_line_gap_spin_->setValue(10.0); // 默认值
  max_gap_layout->addWidget(max_line_gap_spin_);
  line_param_layout->addLayout(max_gap_layout);

  // 为每个工具准备独立的参数区域（Line uses existing controls above）
  // Point tool params
  QWidget* point_param_widget = new QWidget(param_panel);
  QVBoxLayout* point_param_layout = new QVBoxLayout(point_param_widget);
  point_param_layout->setContentsMargins(0,0,0,0);
  QHBoxLayout* maxcorners_layout = new QHBoxLayout();
  maxcorners_layout->addWidget(new QLabel(tr(u8"最大角点数:")));
  point_max_corners_spin_ = new QSpinBox();
  point_max_corners_spin_->setRange(1, 5000);
  point_max_corners_spin_->setValue(500);
  maxcorners_layout->addWidget(point_max_corners_spin_);
  point_param_layout->addLayout(maxcorners_layout);

  QHBoxLayout* quality_layout = new QHBoxLayout();
  quality_layout->addWidget(new QLabel(tr(u8"质量阈值:")));
  point_quality_spin_ = new QDoubleSpinBox();
  point_quality_spin_->setRange(0.001, 1.0);
  point_quality_spin_->setSingleStep(0.001);
  point_quality_spin_->setValue(0.01);
  quality_layout->addWidget(point_quality_spin_);
  point_param_layout->addLayout(quality_layout);

  QHBoxLayout* mindist_layout = new QHBoxLayout();
  mindist_layout->addWidget(new QLabel(tr(u8"最小距离:")));
  point_min_distance_spin_ = new QDoubleSpinBox();
  point_min_distance_spin_->setRange(0.0, 1000.0);
  point_min_distance_spin_->setValue(10.0);
  mindist_layout->addWidget(point_min_distance_spin_);
  point_param_layout->addLayout(mindist_layout);

  // Circle tool params
  QWidget* circle_param_widget = new QWidget(param_panel);
  QVBoxLayout* circle_param_layout = new QVBoxLayout(circle_param_widget);
  circle_param_layout->setContentsMargins(0,0,0,0);
  QHBoxLayout* dp_layout = new QHBoxLayout();
  dp_layout->addWidget(new QLabel(tr(u8"dp:")));
  circle_dp_spin_ = new QDoubleSpinBox();
  circle_dp_spin_->setRange(0.1, 10.0);
  circle_dp_spin_->setValue(1.0);
  dp_layout->addWidget(circle_dp_spin_);
  circle_param_layout->addLayout(dp_layout);

  QHBoxLayout* mindist_c_layout = new QHBoxLayout();
  mindist_c_layout->addWidget(new QLabel(tr(u8"最小圆心距离:")));
  circle_min_dist_spin_ = new QDoubleSpinBox();
  circle_min_dist_spin_->setRange(0.1, 1000.0);
  circle_min_dist_spin_->setValue(20.0);
  mindist_c_layout->addWidget(circle_min_dist_spin_);
  circle_param_layout->addLayout(mindist_c_layout);

  QHBoxLayout* param1_layout = new QHBoxLayout();
  param1_layout->addWidget(new QLabel(tr(u8"param1:")));
  circle_param1_spin_ = new QDoubleSpinBox();
  circle_param1_spin_->setRange(1.0, 500.0);
  circle_param1_spin_->setValue(100.0);
  param1_layout->addWidget(circle_param1_spin_);
  circle_param_layout->addLayout(param1_layout);

  QHBoxLayout* param2_layout = new QHBoxLayout();
  param2_layout->addWidget(new QLabel(tr(u8"param2:")));
  circle_param2_spin_ = new QDoubleSpinBox();
  circle_param2_spin_->setRange(1.0, 500.0);
  circle_param2_spin_->setValue(30.0);
  param2_layout->addWidget(circle_param2_spin_);
  circle_param_layout->addLayout(param2_layout);

  QHBoxLayout* radius_layout = new QHBoxLayout();
  radius_layout->addWidget(new QLabel(tr(u8"最小半径:")));
  circle_min_radius_spin_ = new QSpinBox();
  circle_min_radius_spin_->setRange(0, 1000);
  circle_min_radius_spin_->setValue(0);
  radius_layout->addWidget(circle_min_radius_spin_);
  circle_param_layout->addLayout(radius_layout);

  QHBoxLayout* radius2_layout = new QHBoxLayout();
  radius2_layout->addWidget(new QLabel(tr(u8"最大半径:")));
  circle_max_radius_spin_ = new QSpinBox();
  circle_max_radius_spin_->setRange(0, 10000);
  circle_max_radius_spin_->setValue(0);
  radius2_layout->addWidget(circle_max_radius_spin_);
  circle_param_layout->addLayout(radius2_layout);

  // 执行工具按钮（统一）
  QPushButton* execute_btn = new QPushButton(tr(u8"执行找线"));
  execute_btn->setStyleSheet(R"(
    QPushButton {
      background-color: #1890FF;
      color: white;
      border: none;
      border-radius: 4px;
      padding: 8px;
      font-size: 12px;
    }
    QPushButton:hover {
      background-color: #40A9FF;
    }
    QPushButton:pressed {
      background-color: #096DD9;
    }
  )");
  connect(execute_btn, &QPushButton::clicked, this, &MainWindow::on_execute_tool_clicked);
  param_layout->addWidget(execute_btn);
  // 把参数组添加到 param_layout, 初始仅显示线工具参数
  param_layout->addWidget(line_param_widget);
  param_layout->addWidget(point_param_widget);
  param_layout->addWidget(circle_param_widget);
  line_param_widget_->setVisible(true);
  point_param_widget_ = point_param_widget;
  circle_param_widget_ = circle_param_widget;
  point_param_widget_->setVisible(false);
  circle_param_widget_->setVisible(false);

  // 底部三按钮：确认、取消、应用（暂时确认/取消返回上一级页面，应用暂不实现）
  QHBoxLayout* bottom_btns = new QHBoxLayout();
  bottom_btns->addStretch();
  QPushButton* confirm_btn = new QPushButton(tr(u8"确认"), param_panel);
  QPushButton* cancel_btn = new QPushButton(tr(u8"取消"), param_panel);
  QPushButton* apply_btn = new QPushButton(tr(u8"应用"), param_panel);
  confirm_btn->setFixedWidth(80);
  cancel_btn->setFixedWidth(80);
  apply_btn->setFixedWidth(80);
  bottom_btns->addWidget(apply_btn);
  bottom_btns->addWidget(cancel_btn);
  bottom_btns->addWidget(confirm_btn);
  param_layout->addLayout(bottom_btns);

  // 将工具列表页和参数页加入栈
  element_stack->addWidget(element_list_page);
  element_stack->addWidget(param_panel);

  // 将栈添加到元素页布局
  elem_layout->addWidget(element_stack);

  // 将三个选项卡加入 tab 控件（参数页已移入元素页的栈内）
  tabs->addTab(advanced_tab, tr(u8"高级工具"));
  tabs->addTab(basic_tab, tr(u8"基础工具"));
  tabs->addTab(element_tab, tr(u8"元素工具"));

  // 连接确认/取消按钮：返回上一级（元素工具页）
  connect(confirm_btn, &QPushButton::clicked, this, [this]() {
    if (tabs_ && element_tab_ && element_stack_ && element_list_page_) {
      tabs_->setCurrentWidget(element_tab_);
      element_stack_->setCurrentWidget(element_list_page_);
    }
  });
  connect(cancel_btn, &QPushButton::clicked, this, [this]() {
    if (tabs_ && element_tab_ && element_stack_ && element_list_page_) {
      tabs_->setCurrentWidget(element_tab_);
      element_stack_->setCurrentWidget(element_list_page_);
    }
  });
  // apply 暂不实现具体行为
  connect(apply_btn, &QPushButton::clicked, this, [this]() {
    // no-op for now
  });

  layout->addWidget(tabs);

  return panel;
}

void MainWindow::show_param_for_tool(ToolType t) {
  if (!param_panel_) return;
  // toggle group widgets
  if (line_param_widget_) line_param_widget_->setVisible(t == ToolType::Line);
  if (point_param_widget_) point_param_widget_->setVisible(t == ToolType::Point);
  if (circle_param_widget_) circle_param_widget_->setVisible(t == ToolType::Circle);

  // update execute button text
  if (execute_btn_) {
    if (t == ToolType::Line) execute_btn_->setText(tr(u8"执行找线"));
    else if (t == ToolType::Point) execute_btn_->setText(tr(u8"执行找点"));
    else if (t == ToolType::Circle) execute_btn_->setText(tr(u8"执行找圆"));
  }
}

// ========== 新增：点击找线工具显示参数面板 ==========
void MainWindow::on_find_line_tool_clicked() {
  // 切换到元素页并显示参数配置页面（栈内页面）
  if (tabs_ && element_tab_ && element_stack_ && param_panel_) {
    tabs_->setCurrentWidget(element_tab_);
    element_stack_->setCurrentWidget(param_panel_);
    current_tool_ = ToolType::Line;
    show_param_for_tool(current_tool_);
  } else if (param_panel_) {
    // 兼容：若未使用 tabs，仍然显示面板
    param_panel_->setVisible(true);
  }
}

void MainWindow::on_point_tool_clicked() {
  if (tabs_ && element_tab_ && element_stack_ && param_panel_) {
    tabs_->setCurrentWidget(element_tab_);
    element_stack_->setCurrentWidget(param_panel_);
    current_tool_ = ToolType::Point;
    show_param_for_tool(current_tool_);
  }
}

void MainWindow::on_circle_tool_clicked() {
  if (tabs_ && element_tab_ && element_stack_ && param_panel_) {
    tabs_->setCurrentWidget(element_tab_);
    element_stack_->setCurrentWidget(param_panel_);
    current_tool_ = ToolType::Circle;
    show_param_for_tool(current_tool_);
  }
}

void MainWindow::on_execute_tool_clicked() {
  if (!pixmap_item_) {
    QMessageBox::warning(this, tr(u8"警告"), tr(u8"请先加载图片再执行工具！"));
    return;
  }

  cv::Mat src = qpixmap_to_cvmat(pixmap_item_->pixmap());
  if (src.empty()) return;

  // 如果有ROI，获取并传给工具
  cv::Rect cv_roi;
  if (view_->HasValidRect()) {
    QRectF qt_roi = view_->GetLastDrawRect();
    cv_roi = cv::Rect(static_cast<int>(qt_roi.x()), static_cast<int>(qt_roi.y()), static_cast<int>(qt_roi.width()), static_cast<int>(qt_roi.height()));
  }

  // 根据当前工具创建并运行
  if (current_tool_ == ToolType::Line) {
    tools::LineTool tool;
    tool.params.rho = rho_spin_->value();
    tool.params.theta = theta_spin_->value();
    tool.params.threshold = threshold_spin_->value();
    tool.params.minLineLength = min_line_len_spin_->value();
    tool.params.maxLineGap = max_line_gap_spin_->value();
    tools::DetectionResult res = tool.run(src, cv_roi);
    draw_lines_to_scene(res.lines);
  } else if (current_tool_ == ToolType::Point) {
    tools::PointTool tool;
    tool.params.max_corners = point_max_corners_spin_->value();
    tool.params.quality_level = point_quality_spin_->value();
    tool.params.min_distance = point_min_distance_spin_->value();
    tools::DetectionResult res = tool.run(src, cv_roi);
    draw_points_to_scene(res.points);
  } else if (current_tool_ == ToolType::Circle) {
    tools::CircleTool tool;
    tool.params.dp = circle_dp_spin_->value();
    tool.params.minDist = circle_min_dist_spin_->value();
    tool.params.param1 = circle_param1_spin_->value();
    tool.params.param2 = circle_param2_spin_->value();
    tool.params.minRadius = circle_min_radius_spin_->value();
    tool.params.maxRadius = circle_max_radius_spin_->value();
    tools::DetectionResult res = tool.run(src, cv_roi);
    draw_circles_to_scene(res.circles);
  }
}

// Note: on_execute_find_line_clicked was removed in favor of on_execute_tool_clicked

// ========== 新增：OpenCV 找线核心实现 ==========
// ========== 替换后：仅在ROI矩形内找线的核心实现 ==========
void MainWindow::find_lines_with_opencv() {
  // 第一步：先判断是否绘制了有效矩形，未绘制直接提示（核心ROI校验）
  if (!view_->HasValidRect()) {
    QMessageBox::warning(this, tr(u8"警告"), tr(u8"请先在图片上绘制红色矩形ROI，再执行找线！"));
    return;
  }

  // 1. 将QPixmap转换为cv::Mat（原有逻辑不变）
  cv::Mat src_mat = qpixmap_to_cvmat(pixmap_item_->pixmap());
  if (src_mat.empty()) {
    QMessageBox::critical(this, tr(u8"错误"), tr(u8"图片转换失败！"));
    return;
  }

  // 第二步：获取绘制的矩形ROI坐标（Qt场景坐标 = 图片像素坐标）
  QRectF qt_roi = view_->GetLastDrawRect();
  // 转换为OpenCV的矩形（像素是整数，需强转，保证坐标合法）
  cv::Rect cv_roi(
    static_cast<int>(qt_roi.x()),
    static_cast<int>(qt_roi.y()),
    static_cast<int>(qt_roi.width()),
    static_cast<int>(qt_roi.height())
  );

  // 第三步：ROI边界检查（避免矩形超出图片范围导致OpenCV报错）
  if (cv_roi.x < 0 || cv_roi.y < 0 ||
    cv_roi.x + cv_roi.width > src_mat.cols ||
    cv_roi.y + cv_roi.height > src_mat.rows) {
    QMessageBox::warning(this, tr(u8"警告"), tr(u8"绘制的矩形超出图片范围，请重新绘制！"));
    return;
  }

  // 第四步：截取ROI区域（核心！仅保留矩形内的像素，后续所有处理只针对这个区域）
  cv::Mat roi_mat = src_mat(cv_roi);

  // 2. 预处理：仅对ROI区域做灰度+模糊+边缘检测（把src_mat换成roi_mat）
  cv::Mat gray_mat, edges_mat;
  cv::cvtColor(roi_mat, gray_mat, cv::COLOR_BGR2GRAY);
  cv::GaussianBlur(gray_mat, gray_mat, cv::Size(3, 3), 0); // 高斯模糊去噪
  cv::Canny(gray_mat, edges_mat, 50, 150, 3); // Canny边缘检测

  // 3. 获取参数面板的配置值（原有逻辑不变）
  double rho = rho_spin_->value();
  double theta = theta_spin_->value();
  int threshold = threshold_spin_->value();
  double min_line_len = min_line_len_spin_->value();
  double max_line_gap = max_line_gap_spin_->value();

  // 4. 霍夫直线检测：仅对ROI的边缘图检测（原有逻辑不变）
  std::vector<cv::Vec4i> lines;
  cv::HoughLinesP(edges_mat, lines, rho, theta, threshold, min_line_len, max_line_gap);

  // 第五步：修正直线坐标（关键！ROI内检测的直线是相对坐标，需转回全图绝对坐标）
  for (auto& line : lines) {
    line[0] += cv_roi.x; // 直线起点x + 矩形左偏移
    line[2] += cv_roi.x; // 直线终点x + 矩形左偏移
    line[1] += cv_roi.y; // 直线起点y + 矩形上偏移
    line[3] += cv_roi.y; // 直线终点y + 矩形上偏移
  }

  // 5. 将修正后的直线绘制到Qt场景中（原有逻辑不变）
  draw_lines_to_scene(lines);
}

// ========== 新增：QPixmap 转 cv::Mat ==========
cv::Mat MainWindow::qpixmap_to_cvmat(const QPixmap& pixmap) {
  QImage img = pixmap.toImage();
  img = img.convertToFormat(QImage::Format_RGB888);
  cv::Mat mat(img.height(), img.width(), CV_8UC3, (void*)img.bits(), img.bytesPerLine());
  cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR); // Qt是RGB，OpenCV是BGR，需转换
  return mat.clone(); // 返回深拷贝，避免内存问题
}

// ========== 新增：绘制直线到Qt场景 ==========
void MainWindow::draw_lines_to_scene(const std::vector<cv::Vec4i>& lines) {
  if (lines.empty()) {
    QMessageBox::information(this, tr(u8"提示"), tr(u8"未检测到直线！"));
    return;
  }

  // 遍历所有检测到的直线，创建QGraphicsLineItem绘制
  QPen line_pen(Qt::green, 2); // 绿色直线，宽度2px
  for (const auto& line : lines) {
    int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
    QGraphicsLineItem* line_item = new QGraphicsLineItem(x1, y1, x2, y2);
    line_item->setPen(line_pen);
    scene_->addItem(line_item);
  }

  QMessageBox::information(this, tr(u8"完成"), tr(u8"共检测到 %1 条直线！").arg(lines.size()));
}

// 原有函数（open_image_file、wheelEvent）保持不变
void MainWindow::open_image_file() {
  const QString file_path = QFileDialog::getOpenFileName(
    this, tr(u8"选择图片"), "", tr(u8"图片文件 (*.png *.jpg *.jpeg *.bmp)"));

  if (file_path.isEmpty()) {
    return;
  }

  QPixmap pixmap(file_path);
  if (pixmap.isNull()) {
    setWindowTitle(tr(u8"加载图片失败：") + file_path);
    return;
  }

  if (pixmap_item_) {
    scene_->removeItem(pixmap_item_);
    delete pixmap_item_;
    pixmap_item_ = nullptr;
  }

  pixmap_item_ = scene_->addPixmap(pixmap);
  scene_->setSceneRect(pixmap.rect());
  view_->SetPixmapItem(pixmap_item_);

  setWindowTitle(tr(u8"已加载：") + file_path);
}

void MainWindow::wheelEvent(QWheelEvent* event) {
  if (!view_) {
    return;
  }

  const qreal kScaleFactor = 1.1;
  qreal scale_factor = kScaleFactor;

  if (event->angleDelta().y() < 0) {
    scale_factor = 1.0 / kScaleFactor;
  }

  view_->scale(scale_factor, scale_factor);
}