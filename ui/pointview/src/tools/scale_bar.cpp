/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 3/11/24.
//

#include "scale_bar.h"

ScaleBar::ScaleBar(std::shared_ptr<autox::pointview::DisplayContext> context)
    : autox::pointview::ToolBase(context, "ScaleBar"),
      show_checkbox_(new QCheckBox) {
  show_checkbox_->setObjectName("ShowCheckBox");
  connect(show_checkbox_.get(), SIGNAL(stateChanged(int)), this,
          SLOT(on_ShowCheckBoxStateChanged(int)));
  addProperty("Show", show_checkbox_);
}

ScaleBar::~ScaleBar() {}

void ScaleBar::on_ShowCheckBoxStateChanged(int state) {
  if (state > 0) {
    viewer_->addDistanceScale();
  } else {
    viewer_->removeDistanceScale();
  }
  emit refreshView();
}
