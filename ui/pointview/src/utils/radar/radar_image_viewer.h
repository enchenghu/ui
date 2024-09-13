#ifndef RADAR_IMAGE_VIEWER_H
#define RADAR_IMAGE_VIEWER_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QMouseEvent>
#include <QObject>
#include <QWheelEvent>

#include "dynamic_axis.h"

namespace autox {
namespace pointview {

class RadarImageViewer : public QGraphicsView {
  Q_OBJECT
 public:
  explicit RadarImageViewer(QWidget* parent, bool is_polar_viewer,
                            const int image_height, const int image_width,
                            const double height_resolution,
                            const double width_resolution);

 signals:

 public:
  void setQImage(QImage);
  QImage getQImage();
  void ResetFitView();
  void InitAxis(QGridLayout* grid_layout);

 protected:
  virtual void wheelEvent(QWheelEvent* event) override;
  virtual void keyPressEvent(QKeyEvent* event) override;
  virtual void mousePressEvent(QMouseEvent* event) override;
  virtual void mouseMoveEvent(QMouseEvent* event) override;
  virtual void mouseReleaseEvent(QMouseEvent* event) override;
  virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
  void zoom(QPoint factor);
  void togglePan(bool pan, const QPoint& startPos = QPoint());
  void pan(const QPoint& panTo);
  void initShow();
  void UpdateAxis();

 private:
  QPixmap m_image;
  bool m_isPan;
  QPoint m_prevPan;
  QGraphicsScene* scene;

  int image_height_;
  int image_width_;

  // true for polar viewer and false for cartesian viewer
  bool is_polar_viewer_;
  double height_resolution_;
  double width_resolution_;

  std::shared_ptr<DynamicAxis> y_axis_;
  std::shared_ptr<DynamicAxis> x_axis_;
};

}  // namespace pointview
}  // namespace autox

#endif