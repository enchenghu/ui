//
// Created by tongdayang on 11/29/22.
//

#ifndef POINT_VIEW_GRID_H
#define POINT_VIEW_GRID_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QSpinBox>

#include "utils/common/tool_base.h"
#include "utils/xqtwidget.h"

class Grid : public autox::pointview::ToolBase {
  Q_OBJECT
 public:
  explicit Grid(std::shared_ptr<autox::pointview::DisplayContext> context);
  ~Grid() override;
 private slots:
  void on_ShowCheckBoxStateChanged();
  void on_SizeSpinBoxChanged(int value);
  void on_HeightSpinBoxChanged(double value);

 private:
  void addGridAxisText(const std::string& id, int axis, int value);
  void addGrid(int cell_count, bool show1m, bool show10m, bool show100m);
  void removeGrid();

 private:
  int grid_shape_num_{0};
  int grid_size_ = 200;
  double grid_height_ = 0.0;
  // item
  std::shared_ptr<QCheckBox> grid1m_checkbox_;
  std::shared_ptr<QCheckBox> grid10m_checkbox_;
  std::shared_ptr<QCheckBox> grid100m_checkbox_;
  std::shared_ptr<QSpinBox> size_spinbox_;
  std::shared_ptr<QDoubleSpinBox> height_spinbox_;
};

#endif  // POINT_VIEW_GRID_H
