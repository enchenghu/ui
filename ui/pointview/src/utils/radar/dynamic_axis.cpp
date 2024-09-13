#include "dynamic_axis.h"

#include <QPainter>
#include <iostream>

namespace autox {
namespace pointview {
DynamicAxis::DynamicAxis(bool is_y_axis, const QString& title)
    : is_y_axis_(is_y_axis), title_(title) {
  if (is_y_axis_) {
    setFixedWidth(kAxisSpaceWidth);
  } else {
    setFixedHeight(kAxisSpaceWidth);
  }
}

void DynamicAxis::UpdateAxis(const int start_pixels, const int end_pixels,
                             const float start_value, const float end_value) {
  start_pixels_ = start_pixels;
  end_pixels_ = end_pixels;
  start_value_ = start_value;
  end_value_ = end_value;
  update();
}

void DynamicAxis::paintEvent(QPaintEvent* event) {
  QPainter painter(this);
  painter.setPen(QPen(Qt::black, kLineWidth));

  painter.drawLine(CreateAxisMainLine());

  std::vector<QLine> marker_lines;
  std::vector<QString> marker_strs;
  std::vector<QPoint> strs_pos;
  size_t num = CreateMarkersAndNumbers(marker_lines, marker_strs, strs_pos,
                                       painter.font());
  for (size_t i = 0; i < num; ++i) {
    painter.drawLine(marker_lines[i]);
    painter.drawText(strs_pos[i], marker_strs[i]);
  }

  QPainter title_painter(this);
  title_painter.setPen(QPen(Qt::black, kLineWidth));
  QFont title_font = title_painter.font();
  title_font.setPixelSize(kTitleSize);
  title_painter.setFont(title_font);
  QFontMetrics title_fm(title_font);
  QRect title_str_rect = title_fm.boundingRect(title_);
  if (is_y_axis_) {
    title_painter.translate(0, height());
    title_painter.rotate(-90.0);
    title_painter.drawText(
        QPoint{(height() - title_str_rect.width()) / 2, title_str_rect.height()},
        title_);
  } else {
    title_painter.drawText(QPoint{(width() - title_str_rect.width()) / 2,
                                  height() - title_str_rect.height() / 2},
                           title_);
  }

  title_painter.end();
  painter.end();
}

QLine DynamicAxis::CreateAxisMainLine() {
  if (is_y_axis_) {
    return QLine(kMainAxisLineShift, start_pixels_, kMainAxisLineShift,
                 end_pixels_ - kLineWidth);
  }
  return QLine(start_pixels_, kAxisSpaceWidth - kMainAxisLineShift,
               end_pixels_ - kLineWidth, kAxisSpaceWidth - kMainAxisLineShift);
}

size_t DynamicAxis::CreateMarkersAndNumbers(std::vector<QLine>& marker_lines,
                                            std::vector<QString>& marker_strs,
                                            std::vector<QPoint>& strs_pos,
                                            const QFont& font) {
  size_t num_intervals = static_cast<size_t>(
      static_cast<float>(end_pixels_ - start_pixels_) / kIntervalPixels);
  size_t num_mid_markers = num_intervals == 0 ? 0 : num_intervals - 1;
  marker_lines.clear();
  marker_lines.reserve(num_mid_markers + 2);
  marker_strs.clear();
  marker_strs.reserve(num_mid_markers + 2);
  strs_pos.clear();
  strs_pos.reserve(num_mid_markers + 2);

  for (size_t i = 0; i < num_mid_markers; i++) {
    int marker_p_step =
        (end_pixels_ - start_pixels_) / static_cast<int>(num_intervals);
    float marker_v_step =
        (end_value_ - start_value_) / static_cast<float>(num_intervals);
    int pixel_shift_along_axis = start_pixels_ + marker_p_step * (i + 1);
    float v_shift_along_axis = start_value_ + marker_v_step * (i + 1);
    QLine marker_line;
    QString marker_str = QString::number(v_shift_along_axis, 'g', 3);
    QPoint marker_str_pos;
    if (is_y_axis_) {
      marker_line = {kMainAxisLineShift - kMarkerLineLength,
                     pixel_shift_along_axis, kMainAxisLineShift,
                     pixel_shift_along_axis};
      marker_str_pos = {kMainAxisLineShift - kStrToMainAxisDist,
                        pixel_shift_along_axis};
    } else {
      marker_line = {pixel_shift_along_axis,
                     kAxisSpaceWidth - kMainAxisLineShift,
                     pixel_shift_along_axis,
                     kAxisSpaceWidth - kMainAxisLineShift + kMarkerLineLength};
      marker_str_pos = {
          pixel_shift_along_axis,
          kAxisSpaceWidth - kMainAxisLineShift + kStrToMainAxisDist};
    }
    marker_lines.push_back(marker_line);
    marker_strs.push_back(marker_str);
    strs_pos.push_back(marker_str_pos);
  }
  // endpoints
  QString start_value_str = QString::number(start_value_, 'g', 3);
  QString end_value_str = QString::number(end_value_, 'g', 3);
  marker_strs.emplace_back(start_value_str);
  marker_strs.emplace_back(end_value_str);
  QFontMetrics fm(font);
  if (is_y_axis_) {
    QRect start_v_rect = fm.boundingRect(start_value_str);
    marker_lines.emplace_back(kMainAxisLineShift - kMarkerLineLength,
                              start_pixels_, kMainAxisLineShift, start_pixels_);
    strs_pos.emplace_back(kMainAxisLineShift - kStrToMainAxisDist,
                          start_pixels_ + start_v_rect.height());
    marker_lines.emplace_back(kMainAxisLineShift - kMarkerLineLength,
                              end_pixels_ - kLineWidth, kMainAxisLineShift,
                              end_pixels_ - kLineWidth);
    strs_pos.emplace_back(kMainAxisLineShift - kStrToMainAxisDist, end_pixels_);
  } else {
    QRect end_v_rect = fm.boundingRect(end_value_str);
    marker_lines.emplace_back(
        start_pixels_, kAxisSpaceWidth - kMainAxisLineShift, start_pixels_,
        kAxisSpaceWidth - kMainAxisLineShift + kMarkerLineLength);
    strs_pos.emplace_back(start_pixels_, kAxisSpaceWidth - kMainAxisLineShift +
                                             kStrToMainAxisDist);
    marker_lines.emplace_back(
        end_pixels_ - kLineWidth, kAxisSpaceWidth - kMainAxisLineShift,
        end_pixels_ - kLineWidth,
        kAxisSpaceWidth - kMainAxisLineShift + kMarkerLineLength);
    strs_pos.emplace_back(
        end_pixels_ - end_v_rect.width(),
        kAxisSpaceWidth - kMainAxisLineShift + kStrToMainAxisDist);
  }
  // zero point
  if (start_value_ * end_value_ < 0 &&
      std::find_if(marker_strs.begin(), marker_strs.end(), [](const auto& it) {
        return it == "0";
      }) == marker_strs.end()) {
    float zero_pos_perc = std::abs(start_value_ / (start_value_ - end_value_));
    int zero_pixel_shift_along_axis =
        start_pixels_ +
        static_cast<int>((end_pixels_ - start_pixels_) * zero_pos_perc);
    QLine zero_marker;
    QPoint zero_str_pos;
    if (is_y_axis_) {
      zero_marker = {kMainAxisLineShift - kMarkerLineLength,
                     zero_pixel_shift_along_axis, kMainAxisLineShift,
                     zero_pixel_shift_along_axis};
      zero_str_pos = {kMainAxisLineShift - kStrToMainAxisDist,
                      zero_pixel_shift_along_axis};
    } else {
      zero_marker = {zero_pixel_shift_along_axis,
                     kAxisSpaceWidth - kMainAxisLineShift,
                     zero_pixel_shift_along_axis,
                     kAxisSpaceWidth - kMainAxisLineShift + kMarkerLineLength};
      zero_str_pos = {
          zero_pixel_shift_along_axis,
          kAxisSpaceWidth - kMainAxisLineShift + kStrToMainAxisDist};
    }
    marker_lines.push_back(zero_marker);
    marker_strs.push_back(QString::number(0.0f, 'g', 3));
    strs_pos.push_back(zero_str_pos);
  }
  return marker_lines.size();
}
}  // namespace pointview
}  // namespace autox