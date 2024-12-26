#include "radar_image_viewer.h"

#include <QScrollBar>
#include <iostream>
#include <cmath>

namespace autox {
namespace pointview {
RadarImageViewer::RadarImageViewer(QWidget* parent, bool is_polar_viewer,
                                   const int image_height,
                                   const int image_width,
                                   const double height_resolution,
                                   const double width_resolution)
    : QGraphicsView{parent},
      is_polar_viewer_(is_polar_viewer),
      m_isPan(false),
      m_prevPan(0, 0),
      scene(nullptr),
      image_height_(image_height),
      image_width_(image_width),
      height_resolution_(height_resolution),
      width_resolution_(width_resolution) {
  scene = new QGraphicsScene(this);
  this->setScene(scene);
  this->setMinimumSize(400, 400);

  setDragMode(QGraphicsView::DragMode::NoDrag);
  setInteractive(false);
  setEnabled(true);
}

void RadarImageViewer::setQImage(QImage image) {
  if (image.isNull()) return;
  m_image = QPixmap::fromImage(image);
  if (image_height_ != m_image.height() || image_width_ != m_image.width()) {
    // the first image in
    image_height_ = m_image.height();
    image_width_ = m_image.width();
  }
  initShow();
  if (y_axis_ && x_axis_) {
    UpdateAxis();
  }
}

void RadarImageViewer::initShow() {
  // setEnabled(true);
  setMouseTracking(true);
  scene->clear();
  scene->addPixmap(m_image);
  scene->update();
  this->setSceneRect(m_image.rect());
  UpdateAxis();
}

void RadarImageViewer::UpdateAxis() {
  QPoint img_left_top = mapFromScene(0.0, 0.0);
  QPoint img_right_bottom = mapFromScene(
      {static_cast<float>(image_width_), static_cast<float>(image_height_)});
  QPointF view_left_top_on_scene = mapToScene(0, 0);
  QPointF view_right_bottom_on_scene = mapToScene(width(), height());
  int up = std::max(img_left_top.y(), 0);
  int bottom = std::min(img_right_bottom.y(), height());
  int left = std::max(img_left_top.x(), 0);
  int right = std::min(img_right_bottom.x(), width());

  double up_v, bottom_v, left_v, right_v;
  if (is_polar_viewer_) {
    double azimuth_resolution_degree = height_resolution_ * 180.0 / M_PI;
    double half_azimuth = image_height_ / 2 * azimuth_resolution_degree;
    up_v = up == 0
               ? view_left_top_on_scene.y() * azimuth_resolution_degree - half_azimuth
               : -half_azimuth;
    bottom_v =
        bottom == height()
            ? view_right_bottom_on_scene.y() * azimuth_resolution_degree - half_azimuth
            : half_azimuth;
    left_v = left == 0 ? view_left_top_on_scene.x() * width_resolution_ : 0;
    right_v = right == width()
                  ? view_right_bottom_on_scene.x() * width_resolution_
                  : image_width_ * width_resolution_;
  } else {
    double half_cart_range = image_height_ / 2 * height_resolution_;
    up_v = up == 0 ? half_cart_range -
                         view_left_top_on_scene.y() * height_resolution_
                   : half_cart_range;
    bottom_v = bottom == height()
                   ? half_cart_range -
                         view_right_bottom_on_scene.y() * height_resolution_
                   : -half_cart_range;
    left_v = left == 0 ? half_cart_range -
                             view_left_top_on_scene.x() * height_resolution_
                       : half_cart_range;
    right_v = right == width()
                  ? half_cart_range -
                        view_right_bottom_on_scene.x() * height_resolution_
                  : -half_cart_range;
  }
  y_axis_->UpdateAxis(up, bottom, up_v, bottom_v);
  x_axis_->UpdateAxis(left, right, left_v, right_v);
}

void RadarImageViewer::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    togglePan(true, event->pos());
    event->accept();
    UpdateAxis();
    return;
  }
  event->ignore();
}

void RadarImageViewer::mouseMoveEvent(QMouseEvent* event) {
  if (m_isPan) {
    pan(event->pos());
    event->accept();
    UpdateAxis();
    return;
  }
  event->ignore();
}

void RadarImageViewer::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    togglePan(false);
    event->accept();
    UpdateAxis();
    return;
  }
  event->ignore();
}

void RadarImageViewer::mouseDoubleClickEvent(QMouseEvent* event) {
  ResetFitView();
}

void RadarImageViewer::zoom(QPoint factor) {
  QRectF FOV = this->mapToScene(this->rect()).boundingRect();
  QRectF FOVImage = QRectF(FOV.left(), FOV.top(), FOV.width(), FOV.height());
  float scaleX = static_cast<float>(m_image.width()) / FOVImage.width();
  float scaleY = static_cast<float>(m_image.height()) / FOVImage.height();
  float minScale = scaleX > scaleY ? scaleY : scaleX;
  float maxScale = scaleX > scaleY ? scaleX : scaleY;
  if ((factor.y() > 0 && minScale > 100) || (factor.y() < 0 && maxScale < 1)) {
    return;
  }
  if (factor.y() > 0)
    scale(1.1, 1.1);
  else
    scale(0.9, 0.9);
}
void RadarImageViewer::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_O) {
    ResetFitView();
  }
}

void RadarImageViewer::pan(const QPoint& panTo) {
  auto hBar = horizontalScrollBar();
  auto vBar = verticalScrollBar();
  auto delta = panTo - m_prevPan;
  m_prevPan = panTo;
  hBar->setValue(hBar->value() - delta.x());
  vBar->setValue(vBar->value() - delta.y());
}

void RadarImageViewer::wheelEvent(QWheelEvent* event) {
  if (m_image.isNull()) return;
  QPoint numDegrees = event->angleDelta() / 8;
  if (!numDegrees.isNull()) {
    QPoint numSteps = numDegrees / 15;
    zoom(numSteps);
  }
  event->accept();
  UpdateAxis();
}

void RadarImageViewer::togglePan(bool pan, const QPoint& startPos) {
  if (pan) {
    if (m_isPan) {
      return;
    }
    m_isPan = true;
    m_prevPan = startPos;
    setCursor(Qt::ClosedHandCursor);
  } else {
    if (!m_isPan) {
      return;
    }
    m_isPan = false;
    m_prevPan = QPoint(0, 0);
    setCursor(Qt::ArrowCursor);
  }
}

void RadarImageViewer::ResetFitView() {
  this->resetTransform();
  QRect rect(0, 0, image_width_, image_height_);
  this->setSceneRect(rect);
  this->fitInView(rect, Qt::KeepAspectRatio);
  UpdateAxis();
}

void RadarImageViewer::InitAxis(QGridLayout* grid_layout) {
  QString y_axis_title;
  QString x_axis_title("Range (m)");
  if (is_polar_viewer_) {
    y_axis_title = "Angle (degree)";
  } else {
    y_axis_title = "Range (m)";
  }
  y_axis_.reset(new DynamicAxis(true, y_axis_title));
  grid_layout->addWidget(y_axis_.get(), 0, 0);
  x_axis_.reset(new DynamicAxis(false, x_axis_title));
  grid_layout->addWidget(x_axis_.get(), 1, 1);
  UpdateAxis();
}

}  // namespace pointview
}  // namespace autox