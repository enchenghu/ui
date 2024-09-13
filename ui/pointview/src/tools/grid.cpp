//
// Created by tongdayang on 11/29/22.
//

#include "grid.h"

Grid::Grid(std::shared_ptr<autox::pointview::DisplayContext> context)
    : autox::pointview::ToolBase(context, "Grid"),
      grid1m_checkbox_(new QCheckBox),
      grid10m_checkbox_(new QCheckBox),
      grid100m_checkbox_(new QCheckBox),
      height_spinbox_(new QDoubleSpinBox),
      size_spinbox_(new QSpinBox) {
  // show grid
  connect(grid1m_checkbox_.get(), SIGNAL(stateChanged(int)), this,
          SLOT(on_ShowCheckBoxStateChanged()));
  connect(grid10m_checkbox_.get(), SIGNAL(stateChanged(int)), this,
          SLOT(on_ShowCheckBoxStateChanged()));
  connect(grid100m_checkbox_.get(), SIGNAL(stateChanged(int)), this,
          SLOT(on_ShowCheckBoxStateChanged()));
  addProperty("1m", grid1m_checkbox_);
  addProperty("10m", grid10m_checkbox_);
  addProperty("100m", grid100m_checkbox_);
  // grid size
  size_spinbox_->setSingleStep(100);
  size_spinbox_->setRange(100, 1000);
  size_spinbox_->setSuffix("m");
  size_spinbox_->setValue(grid_size_);
  connect(size_spinbox_.get(), SIGNAL(valueChanged(int)), this,
          SLOT(on_SizeSpinBoxChanged(int)));
  addProperty("side size", size_spinbox_);
  // grid height
  height_spinbox_->setSingleStep(0.1);
  height_spinbox_->setRange(-5.0, 5.0);
  height_spinbox_->setSuffix("m");
  height_spinbox_->setValue(0);
  connect(height_spinbox_.get(), SIGNAL(valueChanged(double)), this,
          SLOT(on_HeightSpinBoxChanged(double)));
  addProperty("height", height_spinbox_);
}

Grid::~Grid() {}

void Grid::addGridAxisText(const std::string& id, int axis, int value) {
  auto text = std::to_string(value) + "m";
  // color and size
  double r, g, b, size;
  if (value % 100 == 0) {
    // 100m
    size = 0.3;
    r = g = 1.0;
    b = 0;
  } else if (value % 10 == 0) {
    // 10m
    size = 0.2;
    r = g = b = 0.8;
  } else {
    // 1m
    size = 0.15;
    r = g = b = 0.6;
  }
  // pose
  pcl::PointXYZ position(0, 0, grid_height_);
  double orientation[3] = {0, 0, 0};
  if (axis == 0) {
    // x
    position.x = value + 0.1;
    orientation[2] = -90;
  } else if (axis == 1) {
    // y
    position.y = value + 0.1;
    orientation[0] = 0;
  }
  viewer_->addText3D(text, position, orientation, size, r, g, b, id);
}

void Grid::addGrid(int cell_count, bool show1m, bool show10m, bool show100m) {
  float cell_size = 1.0;
  double r, g, b;
  int half = cell_count / 2;
  float min = -half * cell_size;
  float max = half * cell_size;
  int cnt = 0;
  for (int i = -half; i <= half; i++) {
    min = -half * cell_size;
    max = half * cell_size;
    // r,g,b
    if (i % 100 == 0 && show100m) {
      // 100m
      r = g = 0.8;
      b = 0;
    } else if (i % 10 == 0 && show10m) {
      // 10m
      r = g = b = 0.5;
    } else {
      if ((!show1m) || abs(i) > 100) {
        continue;
      }
      // only support up to 200*200 cell for 1m
      if (cell_count > 200) {
        min = -100;
        max = 100;
        if (abs(i) > 100) {
          continue;
        }
      }
      // 1m
      r = g = b = 0.3;
    }
    // over x
    viewer_->addLine(pcl::PointXYZ(i * cell_size, min, grid_height_),
                     pcl::PointXYZ(i * cell_size, max, grid_height_), r, g, b,
                     "grid" + std::to_string(cnt));
    cnt++;
    // over y
    viewer_->addLine(pcl::PointXYZ(min, i * cell_size, grid_height_),
                     pcl::PointXYZ(max, i * cell_size, grid_height_), r, g, b,
                     "grid" + std::to_string(cnt));
    cnt++;
    // x axis
    addGridAxisText("grid" + std::to_string(cnt), 0, i * cell_size);
    cnt++;
    // y axis
    if (i != 0) {
      addGridAxisText("grid" + std::to_string(cnt), 1, i * cell_size);
      cnt++;
    }
  }
  grid_shape_num_ = cnt;
}

void Grid::removeGrid() {
  for (int i = 0; i < grid_shape_num_; i++) {
    viewer_->removeShape("grid" + std::to_string(i));
  }
  grid_shape_num_ = 0;
}

void Grid::on_ShowCheckBoxStateChanged() {
  removeGrid();
  addGrid(grid_size_, grid1m_checkbox_->isChecked(),
          grid10m_checkbox_->isChecked(), grid100m_checkbox_->isChecked());
  emit refreshView();
}

void Grid::on_SizeSpinBoxChanged(int value) {
  if (grid_size_ == value) {
    return;
  }
  grid_size_ = value;
  on_ShowCheckBoxStateChanged();
}

void Grid::on_HeightSpinBoxChanged(double value) {
  if (grid_height_ == value) {
    return;
  }
  grid_height_ = value;
  on_ShowCheckBoxStateChanged();
}
