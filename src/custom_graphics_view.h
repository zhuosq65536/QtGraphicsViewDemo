#pragma once

#include <QGraphicsView>
#include <QPointF>
#include <QMouseEvent>
#include <QRectF> // 新增：用于保存矩形坐标

class QGraphicsRectItem;
class QGraphicsPixmapItem;

class CustomGraphicsView : public QGraphicsView {
  Q_OBJECT

public:
  explicit CustomGraphicsView(QWidget* parent = nullptr);
  void SetPixmapItem(QGraphicsPixmapItem* pixmap_item);

  // 新增：获取最后一次绘制的矩形坐标（场景坐标）
  QRectF GetLastDrawRect() const { return last_draw_rect_; }
  // 新增：判断是否绘制了有效矩形
  bool HasValidRect() const { return !last_draw_rect_.isEmpty() && last_draw_rect_.width() > 0 && last_draw_rect_.height() > 0; }

protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  bool is_drawing_ = false;
  QPointF start_scene_pos_;
  QGraphicsRectItem* drawing_rect_;
  QGraphicsPixmapItem* pixmap_item_;

  // 新增：保存最后一次绘制的矩形坐标
  QRectF last_draw_rect_;
};