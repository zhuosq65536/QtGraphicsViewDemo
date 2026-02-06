#include "custom_graphics_view.h"
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>
#include <QPainter>

CustomGraphicsView::CustomGraphicsView(QWidget* parent)
  : QGraphicsView(parent), drawing_rect_(nullptr), pixmap_item_(nullptr) {
  // 视图基础配置（抗锯齿、平移模式）
  setRenderHint(QPainter::Antialiasing);
  setDragMode(QGraphicsView::ScrollHandDrag);
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

// 设置当前图片项（供MainWindow调用）
void CustomGraphicsView::SetPixmapItem(QGraphicsPixmapItem* pixmap_item) {
  pixmap_item_ = pixmap_item;
}

// 鼠标按下：开始绘制矩形
void CustomGraphicsView::mousePressEvent(QMouseEvent* event) {
  // 仅处理左键，且已加载图片时才允许绘制
  if (event->button() != Qt::LeftButton || !pixmap_item_) {
    // 未满足条件时，执行父类逻辑（保证平移等功能正常）
    QGraphicsView::mousePressEvent(event);
    return;
  }

  is_drawing_ = true;
  // 将视图坐标转换为场景坐标（关键：确保矩形画在图片对应位置）
  start_scene_pos_ = mapToScene(event->pos());

  // 创建绘制中的矩形图元（红色边框+半透明红色填充）
  drawing_rect_ = new QGraphicsRectItem();
  drawing_rect_->setPen(QPen(Qt::red, 2));
  drawing_rect_->setBrush(QBrush(QColor(255, 0, 0, 50))); // 50是透明度（0-255）
  scene()->addItem(drawing_rect_);
}

// 鼠标移动：实时更新矩形大小
void CustomGraphicsView::mouseMoveEvent(QMouseEvent* event) {
  if (!is_drawing_ || !drawing_rect_) {
    // 未绘制时，执行父类逻辑（保证平移等功能正常）
    QGraphicsView::mouseMoveEvent(event);
    return;
  }

  // 获取当前鼠标的场景坐标
  QPointF current_scene_pos = mapToScene(event->pos());
  // 计算矩形的左上角、宽高（保证矩形坐标为正）
  qreal x = qMin(start_scene_pos_.x(), current_scene_pos.x());
  qreal y = qMin(start_scene_pos_.y(), current_scene_pos.y());
  qreal width = qAbs(current_scene_pos.x() - start_scene_pos_.x());
  qreal height = qAbs(current_scene_pos.y() - start_scene_pos_.y());

  // 更新矩形图元的位置和大小
  drawing_rect_->setRect(x, y, width, height);
}

// 鼠标释放：结束绘制（保留矩形图元）
void CustomGraphicsView::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() != Qt::LeftButton || !is_drawing_) {
    QGraphicsView::mouseReleaseEvent(event);
    return;
  }

  is_drawing_ = false;
  // 新增：计算并保存最后一次绘制的矩形坐标（场景坐标）
  QPointF current_scene_pos = mapToScene(event->pos());
  qreal x = qMin(start_scene_pos_.x(), current_scene_pos.x());
  qreal y = qMin(start_scene_pos_.y(), current_scene_pos.y());
  qreal width = qAbs(current_scene_pos.x() - start_scene_pos_.x());
  qreal height = qAbs(current_scene_pos.y() - start_scene_pos_.y());
  last_draw_rect_ = QRectF(x, y, width, height); // 保存矩形

  drawing_rect_ = nullptr;
}