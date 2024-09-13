/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 6/20/24.
//

#include "point_data.h"
autox::pointview::PointData::PointData(
    std::shared_ptr<DeviceContext> device_context, QStringList head)
    : PointTable(device_context->getDisplayContext()->getParent(), head,
                 "Point Data"),
      open_button_(new QPushButton(this)) {
  device_context_ = device_context;
  refresh_button_ = std::make_shared<QPushButton>(this);
  refresh_button_->setText("Refresh");
  custom_function_layout_->addWidget(refresh_button_.get());
  connect(refresh_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(on_refresh_buttonClicked()));
  // properties
  open_button_->setText("Open");
  device_context_->addProperty("Point Data", open_button_);
  connect(open_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(on_openButtonClicked()));
  device_context_->addProperties(properties_);
}

void autox::pointview::PointData::update(PointCloudT::Ptr point_cloud) {
  crt_point_cloud_ = point_cloud;
}

void autox::pointview::PointData::on_refresh_buttonClicked() {
  cleanTable();
  std::vector<double> data;
  for (size_t i = 0; i < crt_point_cloud_->size(); i++) {
    data.clear();
    if (point_info_cb_(i, data)) {
      addPoint(data);
    }
  }
  refresh();
}

void autox::pointview::PointData::on_openButtonClicked() {
  this->move(this->parentWidget()->geometry().center() - this->rect().center());
  this->show();
}
