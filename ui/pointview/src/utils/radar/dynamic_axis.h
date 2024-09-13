/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef DYNAMIC_AXIS_H
#define DYNAMIC_AXIS_H

#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>
#include <QTimer>
#include <memory>

namespace autox {
namespace pointview {

class DynamicAxis : public QWidget {
  Q_OBJECT
 public:
  DynamicAxis(bool is_y_axis, const QString& title = "");
  ~DynamicAxis() = default;

  void UpdateAxis(const int start_pixels, const int end_pixels,
                  const float start_value, const float end_value);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  QLine CreateAxisMainLine();
  size_t CreateMarkersAndNumbers(std::vector<QLine>& marker_lines,
                                 std::vector<QString>& marker_strs,
                                 std::vector<QPoint>& strs_pos,
                                 const QFont& font);

 private:
  static constexpr int kLineWidth = 1;
  static constexpr int kAxisSpaceWidth = 60;
  static constexpr int kMainAxisLineShift = 59;
  static constexpr int kMarkerLineLength = 4;
  static constexpr int kStrToMainAxisDist = 35;
  static constexpr float kIntervalPixels = 100.0f;
  static constexpr int kTitleSize = 16;

  // true for vertical y-axis and false for horizontal x-axis
  bool is_y_axis_;
  QString title_;

  int start_pixels_;
  int end_pixels_;
  float start_value_;
  float end_value_;
};

}  // namespace pointview
}  // namespace autox

#endif  // DYNAMIC_AXIS_H