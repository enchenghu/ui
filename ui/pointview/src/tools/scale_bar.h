/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 3/11/24.
//

#ifndef POINTVIEW_SCALE_BAR_H
#define POINTVIEW_SCALE_BAR_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

#include "utils/common/tool_base.h"
#include "utils/xqtwidget.h"

class ScaleBar : public autox::pointview::ToolBase {
  Q_OBJECT
 public:
  explicit ScaleBar(std::shared_ptr<autox::pointview::DisplayContext> context);
  ~ScaleBar() override;
 private slots:
  void on_ShowCheckBoxStateChanged(int state);

 private:
  std::shared_ptr<QCheckBox> show_checkbox_;
};

#endif  // POINTVIEW_SCALE_BAR_H
